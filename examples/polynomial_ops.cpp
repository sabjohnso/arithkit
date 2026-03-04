/// @file polynomial_ops.cpp
/// @brief Polynomial arithmetic, division, and GCD computation.
///
/// Demonstrates Polynomial<T> with exact rational coefficients:
/// arithmetic operations, polynomial long division with remainder,
/// Horner evaluation, and GCD via the Euclidean algorithm.

#include <iostream>

#include <arithkit/fraction.hpp>
#include <arithkit/polynomial.hpp>

using arithkit::Fraction;
using arithkit::Polynomial;

using Q = Fraction<int>;
using P = Polynomial<Q>;

/// Polynomial GCD via iterated Euclidean division.
P
gcd(P a, P b) {
  while (!b.is_zero()) {
    auto r = a % b;
    a = b;
    b = r;
  }
  // Make monic (leading coefficient 1) for canonical form.
  if (!a.is_zero()) {
    auto lead = a[a.degree()];
    auto one = Q(1);
    auto inv_lead = one / lead;
    a = a * inv_lead;
  }
  return a;
}

int main() {
  std::cout << "=== Polynomial Arithmetic over Q ===" << '\n';

  // p(x) = 1 + 2x + 3x²
  P p({Q(1), Q(2), Q(3)});
  // q(x) = 2 + x
  P q({Q(2), Q(1)});

  std::cout << "p(x) = " << p << '\n';
  std::cout << "q(x) = " << q << '\n';
  std::cout << "p + q = " << (p + q) << '\n';
  std::cout << "p * q = " << (p * q) << '\n';

  // Division with remainder
  auto quot = p / q;
  auto rem = p % q;
  std::cout << '\n' << "p / q = " << quot << '\n';
  std::cout << "p % q = " << rem << '\n';
  std::cout << "q*quot + rem = " << (q * quot + rem) << "  (should be "
            << p << ")" << '\n';

  // Evaluation
  std::cout << '\n' << "p(2) = " << p.evaluate(Q(2)) << '\n';

  // GCD: gcd(x²-1, x²+2x+1) = x+1
  // x²-1 = (x-1)(x+1), x²+2x+1 = (x+1)²
  std::cout << '\n' << "=== Polynomial GCD ===" << '\n';
  P a({Q(-1), Q(0), Q(1)});   // x² - 1
  P b({Q(1), Q(2), Q(1)});    // x² + 2x + 1
  std::cout << "a(x) = " << a << '\n';
  std::cout << "b(x) = " << b << '\n';
  std::cout << "gcd(a, b) = " << gcd(a, b) << "  (should be 1 + x)"
            << '\n';

  return 0;
}
