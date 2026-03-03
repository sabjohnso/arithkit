/// @file affine_vs_interval.cpp
/// @brief Affine arithmetic vs Interval: the dependency problem.
///
/// Evaluates f(x) = x - x^2 on [0,1] with both Interval and Affine
/// arithmetic.  Interval treats the two occurrences of x as
/// independent, producing a pessimistic bound.  Affine tracks the
/// correlation via shared noise symbols, giving a tighter result.
///
/// This is the canonical demonstration of the "dependency problem"
/// in interval arithmetic and how affine arithmetic solves it.

#include <iomanip>
#include <iostream>

#include <arithkit/affine.hpp>
#include <arithkit/interval.hpp>

using arithkit::Affine;
using arithkit::Interval;

int
main() {
  std::cout << "=== Affine vs Interval: The Dependency Problem ===\n\n";
  std::cout << "f(x) = x - x^2 on x in [0, 1]\n";
  std::cout << "True range: [0, 0.25]  (maximum at x = 0.5)\n\n";

  // --- Interval arithmetic ---
  Interval<double> x_iv(0.0, 1.0);
  auto f_iv = x_iv - x_iv * x_iv;

  std::cout << "Interval arithmetic:\n";
  std::cout << "  x        = " << x_iv << "\n";
  std::cout << "  x * x    = " << (x_iv * x_iv) << "\n";
  std::cout << "  x - x^2  = " << f_iv << "\n";
  std::cout << "  width    = " << f_iv.width() << "\n\n";

  // --- Affine arithmetic ---
  // x = 0.5 + 0.5*e0  represents [0, 1] with one noise symbol
  Affine<double> x_af(0.5, {0.5});
  auto f_af = x_af - x_af * x_af;
  auto f_af_iv = f_af.to_interval();

  std::cout << "Affine arithmetic:\n";
  std::cout << "  x        = " << x_af << "\n";
  std::cout << "  x * x    = " << (x_af * x_af) << "\n";
  std::cout << "  x - x^2  = " << f_af << "\n";
  std::cout << "  as interval: " << f_af_iv << "\n";
  std::cout << "  width    = " << f_af_iv.width() << "\n\n";

  // --- Comparison ---
  std::cout << std::setprecision(4);
  std::cout << "Summary:\n";
  std::cout << "  True range:     [0, 0.25]     width = 0.25\n";
  std::cout << "  Interval:       " << f_iv << "  width = " << f_iv.width()
            << "\n";
  std::cout << "  Affine:         " << f_af_iv
            << "  width = " << f_af_iv.width() << "\n\n";

  std::cout
    << "The interval result is " << f_iv.width() / f_af_iv.width()
    << "x wider than the affine result.\n\n"
    << "Why: Interval treats the two occurrences of x in 'x - x^2' as\n"
    << "independent variables. Affine tracks that they share noise symbol\n"
    << "e0, so cancellation is properly accounted for.\n";

  return 0;
}
