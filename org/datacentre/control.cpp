// hjb_loader.cpp

#include "control.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <iostream>
//------------------------------------------------------------------------------
// Implementation of load_hjb_solution
//------------------------------------------------------------------------------

HJBData load_hjb_solution(const std::string &filename, const std::string &half) {
    HJBData data;

    // 1) Open the HDF5 file
    H5::H5File file(filename, H5F_ACC_RDONLY);

    // 2) Read each 1‑D grid
    auto read1d = [&](const std::string &name, std::vector<double> &out) {
        H5::DataSet ds   = file.openDataSet(name);
        H5::DataSpace dsp = ds.getSpace();
        hsize_t dims[1];
        dsp.getSimpleExtentDims(dims, nullptr);
        out.resize(dims[0]);
        ds.read(out.data(), H5::PredType::NATIVE_DOUBLE);
    };

    read1d("inv_grid", data.inv_grid);
    read1d("z_grid",   data.z_grid);
    read1d("sig_grid", data.sig_grid);

    data.Nq = data.inv_grid.size();
    data.Nz = data.z_grid.size();
    data.Ns = data.sig_grid.size();

    // 3) Read the 3‑D phi array
    {
        H5::DataSet ds   = file.openDataSet(half);
        H5::DataSpace dsp = ds.getSpace();
        hsize_t dims[3];
        dsp.getSimpleExtentDims(dims, nullptr);

        if ((size_t)dims[0] != data.Nq ||
            (size_t)dims[1] != data.Nz ||
            (size_t)dims[2] != data.Ns) {
            throw std::runtime_error("Dimension mismatch reading phi");
        }

        data.phi.resize(data.Nq * data.Nz * data.Ns);
        ds.read(data.phi.data(), H5::PredType::NATIVE_DOUBLE);
        std::cout << "Loaded φ[0] = " << data.phi[0]
          << ", φ[last] = " << data.phi.back() << "\n"
          << " total elements = " << data.phi.size() << "\n";
    }

    return data;
}

//------------------------------------------------------------------------------
// Helper: find index of nearest value in a sorted vector
//------------------------------------------------------------------------------
static size_t find_nearest(const std::vector<double> &grid, double x) {
    auto it = std::lower_bound(grid.begin(), grid.end(), x);
    if (it == grid.begin())        return 0;
    if (it == grid.end())          return grid.size()-1;
    // compare x   vs  the two neighbours
    size_t idx = std::distance(grid.begin(), it);
    double hi = grid[idx], lo = grid[idx-1];
    return (std::fabs(x - lo) < std::fabs(hi - x)) ? idx-1 : idx;
}

//------------------------------------------------------------------------------
// phi_at: return φ via nearest‐grid lookup
//------------------------------------------------------------------------------
double phi_at(const HJBData &d, double q, double z, double sigma) {
    // clamp into bounds
    q     = std::min(std::max(q,     d.inv_grid.front()), d.inv_grid.back());
    z     = std::min(std::max(z,     d.z_grid.front()),   d.z_grid.back());
    sigma = std::min(std::max(sigma, d.sig_grid.front()), d.sig_grid.back());
    // after clamping and before flattening:


    size_t iq = find_nearest(d.inv_grid,   q);
    size_t iz = find_nearest(d.z_grid,     z);
    size_t is = find_nearest(d.sig_grid,   sigma);

    // flatten index
    size_t idx = (iq * d.Nz + iz) * d.Ns + is;
    std::cout
      << "--- phi_at debug ---\n"
      << " clamped inputs:   q=" << q
      << ", z=" << z
      << ", σ=" << sigma << "\n"
      << " grid indices:     iq=" << iq
      << ", iz=" << iz
      << ", is=" << is << "\n"
      << " grid values:      inv_grid[iq]=" << d.inv_grid[iq]
      << ", z_grid[iz]="   << d.z_grid[iz]
      << ", sig_grid[is]=" << d.sig_grid[is] << "\n"
      << " flattened idx=" << idx
      << ", φ[idx]="      << d.phi[idx] << "\n"
      << "---------------------\n";
    return d.phi[idx];
}
