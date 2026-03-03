/// @file newton_autodiff.cpp
/// @brief Newton's method with automatic differentiation via Dual numbers.
///
/// Finds a root of f(x) = x^3 - 2x - 5 using Newton's method.
/// The derivative f'(x) is computed automatically: evaluating
/// f(Dual(x, 1.0)) yields (f(x), f'(x)) in a single pass.
///
/// No symbolic differentiation.  No finite differences.  Exact
/// derivatives from the algebra of dual numbers.

#include <cmath>
#include <iomanip>
#include <iostream>

#include <arithkit/dual.hpp>

using arithkit::Dual;
using D = Dual<double>;

/// Evaluate f(x) = x^3 - 2x - 5 generically.
/// When T = double, returns the value.
/// When T = Dual<double>, returns (value, derivative).
template <typename T>
auto
f(T x) -> T {
  return x * x * x - T(2.0) * x - T(5.0);
}

int
main() {
  std::cout << "=== Newton's Method with Automatic Differentiation ===\n\n";
  std::cout << "Finding root of f(x) = x^3 - 2x - 5\n\n";

  double x = 2.0; // initial guess
  constexpr double tol = 1e-15;
  constexpr int max_iter = 20;

  std::cout << std::setw(5) << "iter" << std::setw(22) << "x"
            << std::setw(22) << "f(x)" << std::setw(22) << "f'(x)"
            << "\n";
  std::cout << std::string(71, '-') << "\n";

  for (int i = 0; i < max_iter; ++i) {
    // Evaluate f and f' simultaneously using dual numbers
    // Dual(x, 1.0) represents x + 1*epsilon
    auto result = f(D(x, 1.0));
    double fx = result.real();   // f(x)
    double fpx = result.dual();  // f'(x)

    std::cout << std::setw(5) << i << std::setw(22) << std::setprecision(15)
              << x << std::setw(22) << fx << std::setw(22) << fpx << "\n";

    if (std::abs(fx) < tol) {
      std::cout << "\nConverged after " << i << " iterations.\n";
      break;
    }

    x = x - fx / fpx;
  }

  std::cout << "\nRoot: x = " << std::setprecision(15) << x << "\n";
  std::cout << "Verification: f(" << x << ") = " << f(x) << "\n\n";

  // Demonstrate: the same generic f() works for both double and Dual
  std::cout << "--- How it works ---\n";
  std::cout << "f(Dual(2.0, 1.0)) = " << f(D(2.0, 1.0)) << "\n";
  std::cout << "  .real() = " << f(D(2.0, 1.0)).real() << "  (= f(2))\n";
  std::cout << "  .dual() = " << f(D(2.0, 1.0)).dual() << "  (= f'(2))\n";
  std::cout << "\nThe dual part carries the derivative automatically.\n";

  return 0;
}
