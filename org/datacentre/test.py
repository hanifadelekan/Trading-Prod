import numpy as np
from scipy.sparse import diags, kron, eye, csc_matrix
from scipy.sparse.linalg import spsolve
import matplotlib.pyplot as plt

def solve_market_making_hjb(Q_max, s_min, s_max, N_s, sigma_min, sigma_max, N_sigma, 
                            mu_func, eta_func, nu_func, A, k, gamma, 
                            max_policy_iter=100, tol_policy=1e-4, omega=0.5):
    q_grid = np.arange(-Q_max, Q_max + 1)
    N_q = len(q_grid)
    
    s_grid = np.linspace(s_min, s_max, N_s)
    ds = s_grid[1] - s_grid[0]
    
    sigma_grid = np.linspace(sigma_min, sigma_max, N_sigma)
    dsigma = sigma_grid[1] - sigma_grid[0]

    u = np.ones((N_q, N_s, N_sigma))
    delta_bid = np.zeros_like(u)
    delta_ask = np.zeros_like(u)

    # First and second derivative matrices with Neumann BCs
    def neumann_D1(N, dx):
        D = diags([-1, 0, 1], [-1, 0, 1], shape=(N, N)).toarray()
        D[0, :2] = [-1, 1]
        D[-1, -2:] = [-1, 1]
        return D / (2 * dx)

    def neumann_D2(N, dx):
        D = diags([1, -2, 1], [-1, 0, 1], shape=(N, N)).toarray()
        D[0, :2] = [1, -2]
        D[-1, -2:] = [-2, 1]
        return D / (dx ** 2)

    D1_s = neumann_D1(N_s, ds)
    D2_s = neumann_D2(N_s, ds)
    D1_sigma = neumann_D1(N_sigma, dsigma)
    D2_sigma = neumann_D2(N_sigma, dsigma)

    # Build Kronecker operators
    I_s = eye(N_s)
    I_sigma = eye(N_sigma)
    D1_s_kron = kron(I_sigma, csc_matrix(D1_s))
    D2_s_kron = kron(I_sigma, csc_matrix(D2_s))
    D1_sigma_kron = kron(csc_matrix(D1_sigma), I_s)
    D2_sigma_kron = kron(csc_matrix(D2_sigma), I_s)

    for _ in range(max_policy_iter):
        u_old = u.copy()

        for i_q, q in enumerate(q_grid):
            if i_q < N_q - 1:
                ratio_bid = (gamma + k) * u[i_q + 1] / (k * u[i_q] + 1e-10)
                
                delta_bid[i_q] = np.where(ratio_bid > 1, np.log(ratio_bid) / gamma, 0)

            if i_q > 0:
                ratio_ask = (gamma + k) * u[i_q - 1] / (k * u[i_q] + 1e-10)
                delta_ask[i_q] = np.where(ratio_ask > 1, np.log(ratio_ask) / gamma, 0)

        for i_q, q in enumerate(q_grid):
            lambda_bid = A * np.exp(-k * delta_bid[i_q])
            lambda_ask = A * np.exp(-k * delta_ask[i_q])
            phi_bid = np.exp(-gamma * delta_bid[i_q])
            phi_ask = np.exp(-gamma * delta_ask[i_q])

            diag = lambda_bid + lambda_ask
            diag_flat = diag.flatten()

            off_diag_bid = -lambda_bid * phi_bid
            off_diag_ask = -lambda_ask * phi_ask

            mu = np.array([[mu_func(s)] * N_sigma for s in s_grid]).T.flatten()
            eta = np.array([[eta_func(sigma)] * N_s for sigma in sigma_grid]).flatten()
            sigma_vals = np.array([[sigma] * N_s for sigma in sigma_grid])
            nu_vals = np.array([[nu_func(sigma)] * N_s for sigma in sigma_grid])
            sigma_sq = (0.5 * sigma_vals ** 2).flatten()
            nu_sq = (0.5 * nu_vals ** 2).flatten()

            L = diags(mu) @ D1_s_kron + diags(eta) @ D1_sigma_kron \
                + diags(sigma_sq) @ D2_s_kron + diags(nu_sq) @ D2_sigma_kron

            rhs = np.zeros(N_s * N_sigma)
            if i_q < N_q - 1:
                rhs += (off_diag_bid.flatten() * u[i_q + 1].flatten())
            if i_q > 0:
                rhs += (off_diag_ask.flatten() * u[i_q - 1].flatten())

            L_mat = L - diags(diag_flat)
            u_new_flat = spsolve(L_mat, -rhs)
            u_new = u_new_flat.reshape((N_s, N_sigma))
            u[i_q] = omega * u_new + (1 - omega) * u_old[i_q]

        if np.max(np.abs(u - u_old)) < tol_policy:
            break

    return u, delta_bid, delta_ask

# Example usage
if __name__ == "__main__":
    Q_max = 5
    s_min, s_max = 100, 200
    N_s = 50
    sigma_min, sigma_max = 0.1, 0.5
    N_sigma = 30
    A, k, gamma = 1.0, 0.1, 0.01

    mu_func = lambda s: 0.01
    eta_func = lambda sigma: -0.1 * (sigma - 0.2)
    nu_func = lambda sigma: 0.2 * sigma

    u, delta_bid, delta_ask = solve_market_making_hjb(
        Q_max, s_min, s_max, N_s, sigma_min, sigma_max, N_sigma,
        mu_func, eta_func, nu_func, A, k, gamma
    )

    print("Value function shape:", u.shape)
    print("Optimal bid spreads shape:", delta_bid.shape)
    print("Optimal ask spreads shape:", delta_ask.shape)

    # Example visualization
    q_idx = Q_max
    plt.imshow(u[q_idx], aspect='auto', origin='lower',
               extent=[s_min, s_max, sigma_min, sigma_max])
    plt.colorbar(label='Value Function')
    plt.title(f'u(q={q_idx - Q_max})')
    plt.xlabel('Midprice s')
    plt.ylabel('Volatility σ')
    plt.tight_layout()
    plt.show()
