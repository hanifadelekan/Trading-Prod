#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <omp.h>
#include <fstream>

constexpr int N_q = 21;
constexpr int N_s = 50;
constexpr int N_sigma = 30;
constexpr int N_z = 3;

constexpr double q_min = -10;
constexpr double s_min = 90.0;
constexpr double s_max = 110.0;
constexpr double sigma_min = 0.1;
constexpr double sigma_max = 1.0;

constexpr double gamma_coef = 0.1;
constexpr double k = 1.5;
constexpr double A = 1.0;
constexpr double relaxation = 0.5;
constexpr double epsilon = 1e-4;
constexpr int max_iter = 200;

// Drift and volatility dynamics (can be adjusted)
inline double mu(double s, int z, double sigma) { return 0.0; }
inline double eta(double sigma) { return 0.0; }
inline double nu(double sigma) { return 0.1; }

// Safe exponential function to avoid underflow/overflow
inline double safe_exp(double x) {
    if (x > 50.0) return std::exp(50.0);
    if (x < -50.0) return std::exp(-50.0);
    return std::exp(x);
}

int main() {
    const double ds = (s_max - s_min) / (N_s - 1);
    const double d_sigma = (sigma_max - sigma_min) / (N_sigma - 1);

    // Grids
    std::vector<double> q_grid(N_q), s_grid(N_s), sigma_grid(N_sigma);
    for (int i = 0; i < N_q; ++i) q_grid[i] = q_min + i;
    for (int j = 0; j < N_s; ++j) s_grid[j] = s_min + j * ds;
    for (int k = 0; k < N_sigma; ++k) sigma_grid[k] = sigma_min + k * d_sigma;

    // Value function phi[q][s][z][sigma]
    std::vector<std::vector<std::vector<std::vector<double>>>> phi(
        N_q, std::vector<std::vector<std::vector<double>>>(N_s,
            std::vector<std::vector<double>>(N_z, std::vector<double>(N_sigma, 0.0)))
    );

    for (int i_q = 0; i_q < N_q; ++i_q)
    for (int i_s = 0; i_s < N_s; ++i_s)
    for (int i_z = 0; i_z < N_z; ++i_z)
    for (int i_sigma = 0; i_sigma < N_sigma; ++i_sigma)
        phi[i_q][i_s][i_z][i_sigma] = gamma_coef * q_grid[i_q] * s_grid[i_s] / N_q;

    const double C = (1.0 / gamma_coef) * std::log(1.0 + gamma_coef / k);

    // HJB solver loop
    for (int iter = 0; iter < max_iter; ++iter) {
        double max_diff = 0.0;

        #pragma omp parallel for collapse(4) reduction(max:max_diff)
        for (int i_q = 1; i_q < N_q - 1; ++i_q) {
            for (int i_s = 1; i_s < N_s - 1; ++i_s) {
                for (int i_z = 0; i_z < N_z; ++i_z) {
                    for (int i_sigma = 1; i_sigma < N_sigma - 1; ++i_sigma) {

                        double phi_here = phi[i_q][i_s][i_z][i_sigma];

                        // Finite differences
                        double dphi_ds = (phi[i_q][i_s + 1][i_z][i_sigma] - phi[i_q][i_s - 1][i_z][i_sigma]) / (2 * ds);
                        double d2phi_dss = (phi[i_q][i_s + 1][i_z][i_sigma] - 2 * phi_here + phi[i_q][i_s - 1][i_z][i_sigma]) / (ds * ds);

                        double dphi_dsigma = (phi[i_q][i_s][i_z][i_sigma + 1] - phi[i_q][i_s][i_z][i_sigma - 1]) / (2 * d_sigma);
                        double d2phi_dsigma2 = (phi[i_q][i_s][i_z][i_sigma + 1] - 2 * phi_here + phi[i_q][i_s][i_z][i_sigma - 1]) / (d_sigma * d_sigma);

                        double delta_phi_plus = phi[i_q + 1][i_s][i_z][i_sigma] - phi_here;
                        double delta_phi_minus = phi_here - phi[i_q - 1][i_s][i_z][i_sigma];

                        double delta_b = std::min(10.0, C + delta_phi_plus);
                        double delta_a = std::min(10.0, C - delta_phi_minus);

                        double lambda_b = A * std::exp(-k * delta_b);
                        double lambda_a = A * std::exp(-k * delta_a);

                        double J_bid = lambda_b * (safe_exp(-gamma_coef * (delta_b + delta_phi_plus)) - 1.0);
                        double J_ask = lambda_a * (safe_exp(-gamma_coef * (delta_a - delta_phi_minus)) - 1.0);

                        double s = s_grid[i_s];
                        double sigma = sigma_grid[i_sigma];
                        int z = i_z - 1;

                        double drift = mu(s, z, sigma) * dphi_ds;
                        double diffusion = 0.5 * sigma * sigma * d2phi_dss;
                        double vol_term = eta(sigma) * dphi_dsigma + 0.5 * std::pow(nu(sigma), 2) * d2phi_dsigma2;

                        double rhs = -(drift + diffusion + vol_term + J_bid + J_ask);
                        double updated_phi = phi_here + relaxation * (rhs - phi_here);

                        max_diff = std::max(max_diff, std::abs(updated_phi - phi_here));
                        phi[i_q][i_s][i_z][i_sigma] = updated_phi;
                    }
                }
            }
        }

        std::cout << "Iter " << iter << ", max diff = " << max_diff << std::endl;
        if (max_diff < epsilon) break;
    }

    std::cout << "Converged.\n";

    // 🔄 Extract and print optimal quotes
    std::ofstream fout("optimal_quotes.csv");
    fout << "q,s,sigma,z,delta_b,delta_a\n";

    for (int i_q = 1; i_q < N_q - 1; ++i_q)
    for (int i_s = 0; i_s < N_s; ++i_s)
    for (int i_sigma = 0; i_sigma < N_sigma; ++i_sigma)
    for (int i_z = 0; i_z < N_z; ++i_z) {
        double delta_phi_plus = phi[i_q + 1][i_s][i_z][i_sigma] - phi[i_q][i_s][i_z][i_sigma];
        double delta_phi_minus = phi[i_q][i_s][i_z][i_sigma] - phi[i_q - 1][i_s][i_z][i_sigma];

        double delta_b = C + delta_phi_plus;
        double delta_a = C - delta_phi_minus;

        fout << q_grid[i_q] << "," << s_grid[i_s] << "," << sigma_grid[i_sigma] << "," << (i_z - 1) << "," << delta_b << "," << delta_a << "\n";
    }

    fout.close();
    std::cout << "Quotes written to optimal_quotes.csv\n";

    return 0;
}
