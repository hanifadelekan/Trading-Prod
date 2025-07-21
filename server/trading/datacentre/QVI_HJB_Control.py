import numpy as np
from scipy.interpolate import interpn
import time
import matplotlib.pyplot as plt

# --- 1. Model Configuration & Parameters ---

class ModelConfig:
    """
    Holds all parameters for the QVI-HJB model.
    """
    def __init__(self):
        # Market Dynamics & Volatility Process
        self.eta = 1.5        # Signal volatility
        self.kappa_v = 1.0    # Speed of mean reversion for variance
        self.theta_v = 0.04   # Long-run average variance (vol = 20%)
        self.eta_v = 0.2      # Volatility of variance
        
        # Correlations
        self.rho_sz = -0.5    # Correlation between asset and signal
        self.rho_sv = -0.7    # Correlation between asset and variance (leverage effect)
        self.rho_zv = 0.0     # Correlation between signal and variance
        
        # Agent Preferences
        self.rho = 1       # Discount rate (patience)
        self.phi = 0.0001       # Inventory penalty coefficient (risk aversion)
        
        # Fill Intensity Model (lambda = A * size * exp(-B * pos))
        self.A = 0.8          # Base fill rate
        self.B = 1.5          # Queue position decay
        
        # Transaction Costs
        self.K_post = 0.001
        self.K_cancel = 0.0
        self.K_market_pct = 0.004 # 0.4% fee for market orders
        
        # Solver Parameters
        self.dt_solver = 0.01 # Timestep for the iterative solver
        self.convergence_threshold = 1e-6
        self.max_iterations = 2000
        self.omega = 0.008      # Relaxation parameter for stability (0 < omega <= 1)

# --- 2. Grid Setup ---

def setup_grid(q_max, q_steps, z_max, z_steps, v_min, v_max, v_steps, T, t_steps, use_symmetry=False):
    """
    Creates the discretized grid for the state variables.
    """
    q_grid = np.linspace(-q_max, q_max, q_steps)
    v_grid = np.linspace(v_min, v_max, v_steps)
    t_grid = np.linspace(0, T, t_steps)
    
    if use_symmetry:
        z_grid = np.linspace(0, z_max, z_steps // 2 + 1)
        print(f"Symmetry enabled. Solving on reduced z-grid of size {len(z_grid)}.")
    else:
        z_grid = np.linspace(-z_max, z_max, z_steps)
    
    # Create a 4D meshgrid for vectorized operations
    Z, Q, V_grid, T_grid = np.meshgrid(z_grid, q_grid, v_grid, t_grid, indexing='ij')
    
    return {
        "q": q_grid, "z": z_grid, "v": v_grid, "t": t_grid,
        "Q": Q, "Z": Z, "V_grid": V_grid, "T": T_grid,
        "dq": q_grid[1] - q_grid[0],
        "dz": z_grid[1] - z_grid[0],
        "dv": v_grid[1] - v_grid[0],
        "dt": t_grid[1] - t_grid[0]
    }

# --- 3. Differential Operators (Finite Differences) ---

def differentiate(V, grid):
    """
    Calculates all required partial derivatives of the value function V (now 4D)
    """
    dz, dq, dv, dt = grid["dz"], grid["dq"], grid["dv"], grid["dt"]
    
    Vz, Vq, Vv, Vt = np.gradient(V, dz, dq, dv, dt)
    
    Vzz, _, _, _ = np.gradient(Vz, dz, dq, dv, dt)
    Vvv, _, _, _ = np.gradient(Vv, dz, dq, dv, dt)
    
    # Change of variables V = x + qs + v(...) makes these zero
    Vss = np.zeros_like(V)
    Vsz = np.zeros_like(V)
    Vsv = np.zeros_like(V)
    
    # Calculate Vzv
    Vzv, _, _, _ = np.gradient(Vz, dz, dq, dv, dt) # Approximation
    
    return {"Vt": Vt, "Vz": Vz, "Vv": Vv, "Vzz": Vzz, "Vvv": Vvv,
            "Vss": Vss, "Vsz": Vsz, "Vsv": Vsv, "Vzv": Vzv}

# --- 4. QVI Operators ---

def get_continuation_value(V, grid, config, mu_z_func, q_target_func):
    """
    Calculates the value of continuing (LV - running costs) for the stochastic vol model.
    """
    derivs = differentiate(V, grid)
    mu_z = mu_z_func(grid["Z"], grid["T"])
    q_target = q_target_func(grid["Z"], grid["T"])
    
    # --- RIGOROUS IMPLEMENTATION: Explicit Capital Gains Term ---
    capital_gains = grid["Z"] * grid["Q"]
    
    # --- Diffusion Part ---
    # Note: np.sqrt() is applied to grid["V_grid"] as it represents variance (v)
    # The price volatility is sqrt(v).
    
    first_derivs = (mu_z * derivs["Vz"] +
                    config.kappa_v * (config.theta_v - grid["V_grid"]) * derivs["Vv"])
    
    second_derivs = (0.5 * grid["V_grid"] * derivs["Vss"] + # Vss is zero
                     0.5 * config.eta**2 * derivs["Vzz"] +
                     0.5 * config.eta_v**2 * grid["V_grid"] * derivs["Vvv"])
                     
    cross_derivs = (config.rho_sz * np.sqrt(grid["V_grid"]) * config.eta * derivs["Vsz"] + # Vsz is zero
                    config.rho_sv * grid["V_grid"] * config.eta_v * derivs["Vsv"] + # Vsv is zero
                    config.rho_zv * config.eta * config.eta_v * np.sqrt(grid["V_grid"]) * derivs["Vzv"])

    diffusion_val = first_derivs + second_derivs + cross_derivs

    # --- Running Costs ---
    inventory_penalty = config.phi * (grid["Q"] - q_target)**2
    
    return capital_gains + diffusion_val - inventory_penalty

def get_impulse_value(V, grid, config, fixed_states):
    """
    Calculates the value of the best possible impulse action (MV).
    NOTE: This is a simplified placeholder.
    """
    return np.full(V.shape, -np.inf)


# --- 5. Main Solver Loop ---

def solve_qvi_hjb(grid, config, fixed_states, mu_z_func, q_target_func):
    """
    Solves the QVI-HJB equation on the 4D grid.
    """
    print(f"Solving for fixed states: {fixed_states}")
    
    V = np.zeros_like(grid["Q"]) # V is now a 4D array
    
    for i in range(config.max_iterations):
        t_start = time.time()
        V_old = V.copy()
        
        LV_minus_costs = get_continuation_value(V, grid, config, mu_z_func, q_target_func)
        MV = get_impulse_value(V, grid, config, fixed_states)
        
        # Jump terms
        V_after_bid_fill = np.roll(V, shift=-1, axis=1) # q -> q+1
        jump_bid_val = (fixed_states['b'] * config.A * np.exp(-config.B * fixed_states['b_pos']) *
                        (V_after_bid_fill - V))
        
        V_after_ask_fill = np.roll(V, shift=1, axis=1) # q -> q-1
        jump_ask_val = (fixed_states['a'] * config.A * np.exp(-config.B * fixed_states['a_pos']) *
                        (V_after_ask_fill - V))

        hjb_rhs = LV_minus_costs + jump_bid_val + jump_ask_val
        V_candidate = V_old + config.dt_solver * (hjb_rhs - config.rho * V_old)
        
        continuation_mask = hjb_rhs >= MV
        V_update = np.where(continuation_mask, V_candidate, MV)
        V = (1 - config.omega) * V_old + config.omega * V_update

        if np.isnan(V).any():
            print(f"Error: NaN detected in Value Function at iteration {i+1}. Solver failed.")
            return None

        diff = np.max(np.abs(V - V_old))
        t_end = time.time()
        if (i+1) % 100 == 0:
            print(f"Iter: {i+1:04d}, Diff: {diff:.8f}, Time: {t_end - t_start:.4f}s")
        
        if diff < config.convergence_threshold:
            print("\nConvergence reached!")
            break
            
    if i == config.max_iterations - 1:
        print("\nMax iterations reached.")
        
    return V

# --- 7. Plotting Results ---

def plot_results(V, grid, fixed_states):
    """
    Creates plots to visualize the solved 4D value function by taking slices.
    """
    # Find indices for slicing the 4D V array
    q0_idx = np.where(grid["q"] == 0)[0][0]
    t_mid_idx = V.shape[3] // 2
    z_mid_idx = V.shape[0] // 2
    v_mid_idx = V.shape[2] // 2 # Slice at the mid-point of the volatility grid

    fig, axes = plt.subplots(1, 3, figsize=(18, 5))
    fig.suptitle(f"Value Function V for Fixed States: {fixed_states} (at mid-volatility)", fontsize=16)

    # Plot 1: V vs. z (for q=0, v=mid, t=mid)
    axes[0].plot(grid["z"], V[:, q0_idx, v_mid_idx, t_mid_idx])
    axes[0].set_title("Value vs. Signal (at q=0)")
    axes[0].set_xlabel("Signal (z)")
    axes[0].set_ylabel("Value (V)")
    axes[0].grid(True)

    # Plot 2: V vs. q (for z=mid, v=mid, t=mid)
    axes[1].plot(grid["q"], V[z_mid_idx, :, v_mid_idx, t_mid_idx])
    axes[1].set_title("Value vs. Inventory (at mid-signal)")
    axes[1].set_xlabel("Inventory (q)")
    axes[1].set_ylabel("Value (V)")
    axes[1].grid(True)

    # Plot 3: 2D Heatmap of V vs. (z, q) (for v=mid, t=mid)
    V_slice_zq = V[:, :, v_mid_idx, t_mid_idx]
    im = axes[2].imshow(V_slice_zq.T, aspect='auto', origin='lower',
                        extent=[grid["z"][0], grid["z"][-1], grid["q"][0], grid["q"][-1]])
    axes[2].set_title("Value Landscape V(z, q)")
    axes[2].set_xlabel("Signal (z)")
    axes[2].set_ylabel("Inventory (q)")
    fig.colorbar(im, ax=axes[2], label="Value (V)")

    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.show()

# --- 6. Example Usage ---

if __name__ == '__main__':
    def mu_z_func(z, t):
        return 0.0

    def q_target_func(z, t):
        return 0.0

    config = ModelConfig()
    
    # Define the grid dimensions, now including variance 'v'
    grid = setup_grid(q_max=5, q_steps=11, 
                      z_max=0.3, z_steps=31, # Reduced for performance
                      v_min=0.01, v_max=0.4, v_steps=39, # vol from 10% to 40%
                      T=8.0, t_steps=41, # Reduced for performance
                      use_symmetry=True)

    fixed_states_example = {
        'a': 1, 'b': 1,
        'a_pos': 0.2, 'b_pos': 0.2
    }
    
    V_solution_half = solve_qvi_hjb(grid, config, fixed_states_example, mu_z_func, q_target_func)
    
    if V_solution_half is not None:
        print("\n--- Solver Finished ---")
        print(f"Shape of the computed half-solution V: {V_solution_half.shape}")
        plot_results(V_solution_half, grid, fixed_states_example)
