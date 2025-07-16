#!/usr/bin/env python
# --- HJB φ(q,z,σ) solver  •  FEniCSx 0.9  •  no gmsh needed

from mpi4py import MPI
import numpy as np
import dolfinx
from dolfinx import mesh, fem, log, nls, io
from petsc4py import PETSc
import ufl

# ---------- 1. Model params ----------
gamma, k, A = 0.01, 0.30, 0.90
sigma_z, nu  = 0.02, 0.25
Qmax         = 9
jump_const   = A * k / (k - gamma)

# ---------- 2. Rectangle mesh in (z,σ) ----------
Nz, Ns = 100, 50
domain = mesh.create_rectangle(
    MPI.COMM_WORLD,
    [(-0.05, 0.30), (0.05, 0.90)],
    [Nz, Ns],
    cell_type=mesh.CellType.triangle)
V = fem.FunctionSpace(domain, ("CG", 1))
z_, s_ = ufl.SpatialCoordinate(domain)

# ---------- 3. Storage ----------
phi = [fem.Function(V) for _ in range(2*Qmax+1)]
for iq, q in enumerate(range(-Qmax, Qmax+1)):
    phi[iq].interpolate(lambda x: -0.1*q*x[0])   # heuristic tilt

c_plus  = [fem.Function(V) for _ in range(2*Qmax)]
c_minus = [fem.Function(V) for _ in range(2*Qmax)]

# ---------- 4. Helper to solve one (z,σ) PDE ----------
def solve_slice(iq, cp, cm):
    q_val = iq - Qmax
    φ  = phi[iq]
    v  = ufl.TestFunction(V)

    J = jump_const*(cp + cm - 2.0)  # cp,cm already exp(-γΔφ)

    F = ( 0.5*nu**2 * ufl.inner(ufl.grad(φ), ufl.grad(v))*ufl.dx
        + 0.5*sigma_z**2 * ufl.inner(ufl.grad(φ), ufl.grad(v))*ufl.dx
        + (z_*q_val)*v*ufl.dx
        - 0.5*gamma**2*s_**2*q_val**2 * v*ufl.dx
        + J*v*ufl.dx )

    problem = nls.petsc.NonlinearProblem(F, φ)
    solver  = nls.petsc.NewtonSolver(domain.comm, problem)
    solver.rtol = 1e-8; solver.atol = 1e-9
    solver.convergence_criterion = "incremental"
    solver.solve(φ)

# ---------- 5. Howard iteration ----------
outer_tol, outer_max = 1e-6, 25
for outer in range(outer_max):
    # a) policy evaluation
    for iq in range(2*Qmax+1):
        cp = c_plus[iq]   if iq<2*Qmax else fem.Function(V)
        cm = c_minus[iq-1] if iq>0    else fem.Function(V)
        solve_slice(iq, cp, cm)

    # b) policy improvement
    max_dc = 0.0
    for iq in range(2*Qmax):
        δ = fem.Function(V)
        δ.vector.array[:] = phi[iq+1].vector - phi[iq].vector
        new_cp = fem.Function(V); new_cm = fem.Function(V)
        new_cp.vector[:] = np.exp(-gamma*δ.vector.array)
        new_cm.vector[:] = np.exp(+gamma*δ.vector.array)

        max_dc = max(max_dc,
                     np.max(np.abs(new_cp.vector - c_plus[iq].vector)),
                     np.max(np.abs(new_cm.vector - c_minus[iq].vector)))

        c_plus[iq].vector[:]  = new_cp.vector
        c_minus[iq].vector[:] = new_cm.vector

    log.info(f"outer {outer:02d}  max Δc = {max_dc:.2e}")
    if max_dc < outer_tol:
        log.info("Howard converged"); break
else:
    log.warning("hit outer_max")

# ---------- 6. Save q=0 slice ----------
with io.XDMFFile(domain.comm, "phi_q0.xdmf", "w") as xdmf:
    xdmf.write_mesh(domain)
    xdmf.write_function(phi[Qmax])
