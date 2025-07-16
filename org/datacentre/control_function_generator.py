import numpy as np
import matplotlib.pyplot as plt


gamma   = 0.005
k       = 10.0   
A       = 0.3
sigma0  = 0.30         
sigma_z = 0.02          
nu      = 20      

Q          = 9
inv_grid   = np.arange(-Q, Q+1)       
Nq         = len(inv_grid)

Nz         = 101
z_min, z_max = -0.05, 0.05
z_grid     = np.linspace(z_min, z_max, Nz)
dz         = z_grid[1] - z_grid[0]

Ns         = 21
s_min, s_max = 0.1, 2.0
sig_grid   = np.linspace(s_min, s_max, Ns)
ds         = sig_grid[1] - sig_grid[0]

pref = A * k / (k - gamma)


def D2_mat(N, h):
    main = -2*np.ones(N); main[0] = main[-1] = -1
    off  = np.ones(N-1)
    return (np.diag(main) + np.diag(off,1) + np.diag(off,-1)) / (h*h)

D2_sigma = D2_mat(Ns, ds)

phi   = np.zeros((Nq, Nz, Ns))
relax = 0.00001
tol   = 1e-6
max_iter = 1000
clip  = 10.0


for it in range(max_iter):
    phi_old = phi.copy()

    phi_s  = np.zeros_like(phi)
    phi_s[:,:,1:-1] = (phi[:,:,2:] - phi[:,:,:-2]) / (2*ds)
    phi_s[:,:,0]  = phi_s[:,:,1]
    phi_s[:,:,-1] = phi_s[:,:,-2]
    nonlin_sigma = 0.5 * nu**2 * gamma * phi_s**2  
    
    # ---- z derivatives ----
    phi_z  = np.zeros_like(phi)
    phi_z[:,1:-1,:] = (phi[:,2:,:] - phi[:,:-2,:]) / (2*dz)
    phi_z[:,0,:]  = phi_z[:,1,:]
    phi_z[:,-1,:] = phi_z[:,-2,:]
    nonlin_z = 0.5 * sigma_z**2 * gamma * phi_z**2  # shape (Nq,Nz,Ns)
    
    phi_zz = np.zeros_like(phi)
    phi_zz[:,1:-1,:] = (phi[:,2:,:] - 2*phi[:,1:-1,:] + phi[:,:-2,:]) / (dz*dz)
    # Neumann BC gives zero second deriv at edges
    lin_z = -0.5 * sigma_z**2 * phi_zz
    
    # main sweep over q,z
    for iq, q in enumerate(inv_grid):
        for jz, z in enumerate(z_grid):
            row_sigma = phi[iq, jz]                # vector length Ns
            
            # inventory jumps
            delta_p = phi[iq+1, jz] - row_sigma if iq < Nq-1 else np.inf
            delta_m = phi[iq-1, jz] - row_sigma if iq > 0    else np.inf
            
            # clip for exponent
            if np.isfinite(delta_p).any():
                delta_p = np.where(np.isfinite(delta_p),
                                   np.clip(delta_p, -clip, clip),
                                   delta_p)
            if np.isfinite(delta_m).any():
                delta_m = np.where(np.isfinite(delta_m),
                                   np.clip(delta_m, -clip, clip),
                                   delta_m)
            
            drift = z * q
            risk  = -0.5 * (gamma**2) * (sig_grid**2) * q**2
            
            # jump vectors
            e_p = np.where(np.isfinite(delta_p),
                           np.exp(-gamma*delta_p) - 1.0,
                           0.0)
            e_m = np.where(np.isfinite(delta_m),
                           np.exp(-gamma*delta_m) - 1.0,
                           0.0)
            jump = pref * (e_p + e_m)
            
            # linear sigma diffusion term
            lin_sigma = -0.5 * nu**2 * (D2_sigma @ row_sigma)
            
            residual = (drift
                        + risk
                        + jump
                        + nonlin_sigma[iq, jz]
                        + lin_sigma
                        + nonlin_z[iq, jz]
                        + lin_z[iq, jz])
            
            phi[iq, jz] = row_sigma - relax * residual
    
    # anchor at q=0
    kappa = 0.0001
    phi[0,:,:] = phi[1,:,:] + kappa
    phi[-1,:,:] = phi[-2,:,:] - kappa

    
    err = np.max(np.abs(phi - phi_old))
    if err < tol:
        print(f"Converged in {it+1} sweeps, max Δφ = {err:.1e}")
        break
else:
    print("Reached max_iter without full convergence; max Δφ = {:.1e}".format(err))
import h5py
with h5py.File("hjb_solution.h5","w") as f:
    f.create_dataset("phi", data=phi)
    f.create_dataset("inv_grid", data=inv_grid)
    f.create_dataset("z_grid", data=z_grid)
    f.create_dataset("sig_grid", data=sig_grid)

# ======================================================
# 6. Plot mid‑sigma slice
# ======================================================
mid_s = Ns // 3
plt.figure(figsize=(6.5,4))
for z,color in zip([-0.05,0.0,0.05],['orange','blue','red']):
    jz = np.argmin(np.abs(z_grid - z))
    plt.plot(inv_grid, phi[:, jz, mid_s], marker='o', color=color,
             label=f"z={z:+.2f}, σ≈{sig_grid[mid_s]:.2f}")
plt.axhline(0,color='k',lw=0.8)
plt.xlabel("Inventory q"); plt.ylabel("φ (mid σ slice)")
plt.legend(); plt.grid(True, ls='--', alpha=0.6); plt.tight_layout()


# -----------------------------------------------
# after φ has converged
# -----------------------------------------------
delta_p = np.full_like(phi, np.inf)         # q → q + 1  (ask)
delta_m = np.full_like(phi, np.inf)         # q → q - 1  (bid)

delta_p[:-1, :, :] = phi[1:, :, :] - phi[:-1, :, :]   # valid up to q = +Q-1
delta_m[1:,  :, :] = phi[:-1, :, :] - phi[1:,  :, :]  # valid down to q = –Q+1

with h5py.File("hjb_solution.h5", "w") as f:
    f.create_dataset("delta_p",  data=delta_p,  compression="gzip")
    f.create_dataset("delta_m",  data=delta_m,  compression="gzip")
    f.create_dataset("inv_grid", data=inv_grid)
    f.create_dataset("z_grid",   data=z_grid)
    f.create_dataset("sig_grid", data=sig_grid)
    # (optionally keep "phi" as well, but you don't have to)
print(phi[1:-1,:,:] - phi[0:-2,:,:])

# assume phi.shape == (Nq, Nz, Ns)
Nq, Nz, Ns = phi.shape

# -------------------------------------------------------------------
# 1) your interior forward‑difference (ask side, q -> q+1)
# -------------------------------------------------------------------
#    shape will be (Nq-2, Nz, Ns)
inner_dp = phi[2:  , :, :] - phi[1:-1, :, :]   # iq = 1 … Nq-2
inner_dm = phi[0:-2  , :, :] - phi[1:-1, :, :] 
# -------------------------------------------------------------------
# 2) build a full‑size container and insert the interior
# -------------------------------------------------------------------
delta_p = np.empty_like(phi)      # (Nq, Nz, Ns)
delta_p[1:-1, :, :] = inner_dp    # fill iq = 1…Nq-2

delta_p = np.empty_like(phi)      # (Nq, Nz, Ns)
delta_p[1:-1, :, :] = inner_dp 

# -------------------------------------------------------------------
# 3) add the two edge layers however you want
# -------------------------------------------------------------------
# 3a.  UNREACHABLE ⇒ set to NaN
BIG = 1
delta_m[0 , :, :]  = +BIG     # selling lowers value a lot
delta_p[0 , :, :]  = -BIG     # buying raises value a lot

# q = +Q  (already long)      → forbid further buys
delta_p[-1, :, :] = +BIG      # buying lowers value a lot
delta_m[-1, :, :] = -BIG      # iq = Nq-1 (q = +Q): can't take q+1
# ----- OR choose one of these alternatives instead of NaN -----

# 3b.  COPY nearest interior value  (flat extrapolation)
# delta_p[0 , :, :] = delta_p[ 1, :, :]
# delta_p[-1, :, :] = delta_p[-2, :, :]

# 3c.  BIG constant to force a huge spread
# delta_p[0 , :, :] = 1e9
# delta_p[-1, :, :] = 1e9


g_over_k_minus_g = gamma / (k - gamma)

# ask (buy one lot)          δ⁺ large +ve  → wide spread (discourage)
ask_half = -(1.0/gamma) * (
    np.log1p( g_over_k_minus_g * np.exp(-gamma * delta_p) )
)

# bid (sell one lot)         δ⁻ large +ve → wide spread (discourage)
bid_half = -(1.0/gamma) * (
    np.log1p( g_over_k_minus_g * np.exp(-gamma * delta_m) )
)

'''ask_half = np.clip(ask_half, -BIG, BIG)
bid_half = np.clip(bid_half, -BIG, BIG)'''

# ---------------------------------------------------------
# save only what the quote engine needs
# ---------------------------------------------------------
with h5py.File("hjb_solution.h5", "w") as f:
    f.create_dataset("ask_half", data=ask_half, compression="gzip")
    f.create_dataset("bid_half", data=bid_half, compression="gzip")
    f.create_dataset("inv_grid", data=inv_grid)
    f.create_dataset("z_grid",   data=z_grid)
    f.create_dataset("sig_grid", data=sig_grid)

plt.show()