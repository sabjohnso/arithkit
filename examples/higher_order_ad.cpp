/// @file higher_order_ad.cpp
/// @brief Higher-order automatic differentiation with Jet<T,N>.
///
/// Computes f(x), f'(x), f''(x), f'''(x) of a polynomial in a
/// single evaluation pass using Jet<double,3>.  Verifies against
/// known analytic derivatives.

#include <cmath>
#include <iomanip>
#include <iostream>

#include <arithkit/jet.hpp>

using arithkit::Jet;
using J = Jet<double, 3>;

/// f(x) = x^4 - 3x^2 + 2x + 1
/// f'(x) = 4x^3 - 6x + 2
/// f''(x) = 12x^2 - 6
/// f'''(x) = 24x
template <typename T>
auto
f(T x) -> T {
  return x * x * x * x - T(3.0) * x * x + T(2.0) * x + T(1.0);
}

int main() {
  // Seed the Jet at x=2.0 with unit first derivative.
  // coefficients = {x, 1, 0, 0} meaning:
  //   c[0] = value = 2.0
  //   c[1] = dx/dx = 1.0  (first derivative seed)
  //   c[2] = 0             (second derivative seed)
  //   c[3] = 0             (third derivative seed)
  auto x = J(std::array<double, 4>{2.0, 1.0, 0.0, 0.0});
  auto result = f(x);

  // The Jet coefficients hold f^(k)(a)/k!
  double val = result[0];
  double d1 = result[1];           // f'(a) / 1!
  double d2 = result[2] * 2.0;    // f''(a) / 2! * 2!
  double d3 = result[3] * 6.0;    // f'''(a) / 3! * 3!

  // Analytic values at x=2
  double exact_f = 9.0;     // 16 - 12 + 4 + 1
  double exact_d1 = 22.0;   // 32 - 12 + 2
  double exact_d2 = 42.0;   // 48 - 6
  double exact_d3 = 48.0;   // 24*2

  std::cout << std::fixed << std::setprecision(6);
  std::cout << "=== Higher-Order AD with Jet<double,3> ===" << '\n';
  std::cout << "f(x) = x^4 - 3x^2 + 2x + 1" << '\n';
  std::cout << "Evaluating at x = 2.0:" << '\n' << '\n';

  auto check = [](const char* label, double got, double expected) {
    bool ok = std::abs(got - expected) < 1e-10;
    std::cout << std::setw(12) << label << " = " << std::setw(10) << got
              << "  (expected " << expected << ")  "
              << (ok ? "OK" : "MISMATCH") << '\n';
  };

  check("f(2)", val, exact_f);
  check("f'(2)", d1, exact_d1);
  check("f''(2)", d2, exact_d2);
  check("f'''(2)", d3, exact_d3);

  return 0;
}
