/// @file hilbert_matrix.cpp
/// @brief Exact Hilbert matrix computation using Fraction<BigInt>.
///
/// The Hilbert matrix H(i,j) = 1/(i+j+1) is notoriously
/// ill-conditioned.  Double-precision floating point loses digits
/// rapidly when computing H*x, while exact fractions remain precise.
///
/// This example constructs a 10x10 Hilbert matrix, multiplies it by
/// a known vector, and compares exact vs floating-point results.

#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

#include <arithkit/bigint.hpp>
#include <arithkit/fraction.hpp>

using arithkit::BigInt;
using arithkit::Fraction;

using Q = Fraction<BigInt>;

auto
hilbert_exact(int n) -> std::vector<std::vector<Q>> {
  std::vector<std::vector<Q>> H(
    static_cast<std::size_t>(n),
    std::vector<Q>(static_cast<std::size_t>(n)));
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      H[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)] =
        Q(BigInt(1), BigInt(i + j + 1));
    }
  }
  return H;
}

auto
matvec_exact(const std::vector<std::vector<Q>>& H, const std::vector<Q>& x)
  -> std::vector<Q> {
  auto n = H.size();
  std::vector<Q> result(n, Q(BigInt(0)));
  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = 0; j < n; ++j) {
      result[i] += H[i][j] * x[j];
    }
  }
  return result;
}

auto
matvec_double(int n, const std::vector<double>& x) -> std::vector<double> {
  std::vector<double> result(static_cast<std::size_t>(n), 0.0);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      result[static_cast<std::size_t>(i)] +=
        1.0 / (i + j + 1) * x[static_cast<std::size_t>(j)];
    }
  }
  return result;
}

auto
to_double(const Q& q) -> double {
  // Approximate: convert numerator and denominator to double
  auto num_str = to_string(q.numerator());
  auto den_str = to_string(q.denominator());
  return std::stod(num_str) / std::stod(den_str);
}

int
main() {
  constexpr int n = 10;

  std::cout << "=== Exact Hilbert Matrix with Fraction<BigInt> ===\n\n";

  // Build exact Hilbert matrix
  auto H = hilbert_exact(n);

  // Choose x = [1, 1, ..., 1]
  std::vector<Q> x_exact(static_cast<std::size_t>(n), Q(BigInt(1)));
  std::vector<double> x_double(static_cast<std::size_t>(n), 1.0);

  // Compute H*x exactly and in double
  auto b_exact = matvec_exact(H, x_exact);
  auto b_double = matvec_double(n, x_double);

  std::cout << "H * [1,1,...,1] for " << n << "x" << n
            << " Hilbert matrix:\n\n";
  std::cout << std::setw(4) << "i" << std::setw(30) << "Exact (fraction)"
            << std::setw(22) << "double" << std::setw(16) << "error\n";
  std::cout << std::string(72, '-') << "\n";

  for (int i = 0; i < n; ++i) {
    auto exact_val = to_double(b_exact[static_cast<std::size_t>(i)]);
    auto dbl_val = b_double[static_cast<std::size_t>(i)];
    auto err = std::abs(exact_val - dbl_val);

    std::cout << std::setw(4) << i << std::setw(30)
              << b_exact[static_cast<std::size_t>(i)] << std::setw(22)
              << std::setprecision(15) << dbl_val << std::setw(16)
              << std::scientific << err << "\n"
              << std::fixed;
  }

  std::cout << "\nNote: errors are small for H*[1,...,1] since each row\n"
            << "sum is a harmonic partial sum. The ill-conditioning\n"
            << "becomes catastrophic when solving H*x = b (inverting H).\n\n";

  // Demonstrate: exact computation of sum(1/k) for k=1..n
  Q harmonic(BigInt(0));
  for (int k = 1; k <= n; ++k) {
    harmonic += Q(BigInt(1), BigInt(k));
  }
  std::cout << "Exact H(10) = " << harmonic << "\n";
  std::cout << "        ≈ " << std::setprecision(15) << to_double(harmonic)
            << "\n";

  return 0;
}
