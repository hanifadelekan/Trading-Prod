// hjb_loader.h

#pragma once
#include <string>
#include <vector>
#include "H5Cpp.h"

// Simple POD to hold grids and φ
struct HJBData {
    std::vector<double> phi;      // size = Nq * Nz * Ns
    std::vector<double> inv_grid; // size = Nq
    std::vector<double> z_grid;   // size = Nz
    std::vector<double> sig_grid; // size = Ns
    size_t Nq, Nz, Ns;            // dimensions
};

/**
 * Load the HJB solution from an HDF5 file with datasets:
 *   "phi"      : float64 [Nq x Nz x Ns]
 *   "inv_grid" : float64 [Nq]
 *   "z_grid"   : float64 [Nz]
 *   "sig_grid" : float64 [Ns]
 *
 * @param filename Path to the .h5 file
 * @returns HJBData struct with the arrays and dimensions
 * @throws runtime_error on dimension mismatch or file errors
 */
HJBData load_hjb_solution(const std::string &filename="hjb_solution.h5", const std::string &half= "");

/**
 * Query φ at an arbitrary state (q,z,σ) by nearest‐grid lookup.
 * Out‐of‐bounds values are clamped to the grid edges.
 *
 * @param d      The loaded HJBData
 * @param q      Inventory state
 * @param z      Signal state
 * @param sigma  Volatility state
 * @returns      φ(q,z,σ) approximated at nearest grid node
 */
double phi_at(const HJBData &d, double q, double z, double sigma);
