/// @file composition_algebra.cpp
/// @brief Octonion composition algebra property and non-associativity.
///
/// Demonstrates that octonions form a composition algebra:
///   ||a*b|| = ||a|| * ||b||
/// This is the defining property of normed division algebras.
/// By Hurwitz's theorem, only R, C, H, and O satisfy this.
///
/// Also demonstrates non-associativity with a concrete counterexample.

#include <cmath>
#include <iomanip>
#include <iostream>

#include <arithkit/octonion.hpp>

using arithkit::Octonion;
using O = Octonion<double>;

int
main() {
  std::cout << "=== Octonion Composition Algebra ===\n\n";

  // --- Composition property: norm(a*b) == norm(a) * norm(b) ---
  std::cout << "--- Composition property: ||a*b|| = ||a|| * ||b|| ---\n\n";

  O a(1.0, 2.0, -1.0, 3.0, 0.5, -2.0, 1.5, -0.5);
  O b(-1.0, 0.0, 3.0, -2.0, 1.0, 1.0, -1.0, 2.0);

  auto ab = a * b;

  double norm_a = norm(a);
  double norm_b = norm(b);
  double norm_ab = norm(ab);
  double product_of_norms = norm_a * norm_b;

  std::cout << "a = " << a << "\n";
  std::cout << "b = " << b << "\n\n";
  std::cout << std::setprecision(10);
  std::cout << "||a||      = " << std::sqrt(norm_a) << "  (norm squared = "
            << norm_a << ")\n";
  std::cout << "||b||      = " << std::sqrt(norm_b) << "  (norm squared = "
            << norm_b << ")\n";
  std::cout << "||a*b||    = " << std::sqrt(norm_ab) << "  (norm squared = "
            << norm_ab << ")\n";
  std::cout << "||a||*||b||= " << std::sqrt(product_of_norms)
            << "  (product   = " << product_of_norms << ")\n";
  std::cout << "Match: " << (std::abs(norm_ab - product_of_norms) < 1e-10 ? "yes" : "no")
            << "\n\n";

  // --- Non-associativity ---
  std::cout << "--- Non-associativity ---\n\n";

  // Use basis elements: e1, e2, e4
  O e1(0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  O e2(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  O e4(0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0);

  auto lhs = (e1 * e2) * e4; // (e1*e2)*e4
  auto rhs = e1 * (e2 * e4); // e1*(e2*e4)

  std::cout << "Testing (e1 * e2) * e4  vs  e1 * (e2 * e4):\n\n";
  std::cout << "  e1 * e2   = " << (e1 * e2) << "\n";
  std::cout << "  e2 * e4   = " << (e2 * e4) << "\n";
  std::cout << "  (e1*e2)*e4 = " << lhs << "\n";
  std::cout << "  e1*(e2*e4) = " << rhs << "\n";
  std::cout << "  Equal? " << (lhs == rhs ? "yes" : "NO — non-associative!")
            << "\n\n";

  // --- But alternativity holds ---
  std::cout << "--- Alternativity (weaker than associativity) ---\n\n";

  O p(1.0, 2.0, -1.0, 3.0, 0.5, -2.0, 1.5, -0.5);
  O q(-1.0, 0.0, 3.0, -2.0, 1.0, 1.0, -1.0, 2.0);

  auto left_alt_lhs = p * (p * q);
  auto left_alt_rhs = (p * p) * q;
  bool left_ok = true;
  for (std::size_t i = 0; i < 8; ++i) {
    if (std::abs(left_alt_lhs[i] - left_alt_rhs[i]) > 1e-8) left_ok = false;
  }

  auto right_alt_lhs = (p * q) * q;
  auto right_alt_rhs = p * (q * q);
  bool right_ok = true;
  for (std::size_t i = 0; i < 8; ++i) {
    if (std::abs(right_alt_lhs[i] - right_alt_rhs[i]) > 1e-8)
      right_ok = false;
  }

  std::cout << "Left alternativity:   a*(a*b) == (a*a)*b ?  "
            << (left_ok ? "yes" : "no") << "\n";
  std::cout << "Right alternativity:  (a*b)*b == a*(b*b) ?  "
            << (right_ok ? "yes" : "no") << "\n\n";

  // --- Division ---
  std::cout << "--- Division (every nonzero octonion is invertible) ---\n\n";
  auto a_div_a = a / a;
  auto one = O(1.0);
  bool div_ok = true;
  for (std::size_t i = 0; i < 8; ++i) {
    if (std::abs(a_div_a[i] - one[i]) > 1e-10) div_ok = false;
  }
  std::cout << "a / a = " << a_div_a << "\n";
  std::cout << "Equals 1? " << (div_ok ? "yes" : "no") << "\n\n";

  std::cout << "By Hurwitz's theorem (1898), the only normed division\n"
            << "algebras over R are: R (dim 1), C (dim 2), H (dim 4),\n"
            << "and O (dim 8). The octonions are the last in this chain.\n";

  return 0;
}
