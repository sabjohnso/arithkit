/// @file batch_polynomial.cpp
/// @brief Batch polynomial evaluation using Simd<double,4>.
///
/// Evaluates a polynomial at 4 points simultaneously using Horner's
/// method on SIMD vectors.  The same code that evaluates at a single
/// point works unchanged on a SIMD vector — the arithmetic operators
/// handle the lanes transparently.

#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

#include <arithkit/simd.hpp>

using arithkit::Simd;
using S4 = Simd<double, 4>;

/// Evaluate polynomial using Horner's method.
/// coeffs = {a0, a1, ..., an} represents a0 + a1*x + ... + an*x^n
/// Works for T = double (scalar) or T = Simd<double,4> (batch).
template <typename T>
auto
horner(const std::vector<double>& coeffs, T x) -> T {
  T result(coeffs.back());
  for (auto i = coeffs.size() - 1; i-- > 0;) {
    result = result * x + T(coeffs[i]);
  }
  return result;
}

int
main() {
  std::cout << "=== Batch Polynomial Evaluation with Simd<double,4> ===\n\n";

  // p(x) = 1 - x^2/2 + x^4/24  (Taylor approximation of cos)
  std::vector<double> coeffs = {1.0, 0.0, -0.5, 0.0, 1.0 / 24.0};

  std::cout << "p(x) = 1 - x^2/2 + x^4/24  (Taylor approx of cos)\n\n";

  // Evaluate at 4 points simultaneously
  S4 x(0.0, 0.5, 1.0, 1.5);
  auto result = horner(coeffs, x);

  std::cout << "Batch evaluation (4 points at once):\n";
  std::cout << std::setw(10) << "x" << std::setw(16) << "p(x)"
            << std::setw(16) << "cos(x)" << std::setw(16) << "error\n";
  std::cout << std::string(58, '-') << "\n";

  std::array<double, 4> xs = {0.0, 0.5, 1.0, 1.5};
  for (std::size_t i = 0; i < 4; ++i) {
    double exact = std::cos(xs[i]);
    double approx = result[i];
    std::cout << std::setw(10) << std::setprecision(4) << xs[i]
              << std::setw(16) << std::setprecision(10) << approx
              << std::setw(16) << exact << std::setw(16) << std::scientific
              << std::abs(approx - exact) << std::fixed << "\n";
  }

  // Verify: scalar Horner gives the same results
  std::cout << "\nVerification: scalar evaluation matches:\n";
  bool all_match = true;
  for (std::size_t i = 0; i < 4; ++i) {
    double scalar_result = horner(coeffs, xs[i]);
    if (scalar_result != result[i]) {
      std::cout << "MISMATCH at x=" << xs[i] << "\n";
      all_match = false;
    }
  }
  if (all_match) {
    std::cout << "  All 4 scalar results match the SIMD result.\n";
  }

  // Demonstrate: SIMD arithmetic is transparent
  std::cout << "\n--- How it works ---\n";
  std::cout << "The SAME horner() function works for both:\n";
  std::cout << "  horner(coeffs, 1.0)           -> " << std::setprecision(10)
            << horner(coeffs, 1.0) << "\n";
  std::cout << "  horner(coeffs, S4(0,0.5,1,1.5)) -> " << horner(coeffs, x)
            << "\n";
  std::cout << "\nSame algorithm, same code, 4x the throughput.\n"
            << "Arithmetic operators handle lanes transparently.\n";

  return 0;
}
