/// @file modular_arithmetic.cpp
/// @brief Modular arithmetic demo: exponentiation and Diffie-Hellman.
///
/// Demonstrates ModInt<N> with a toy Diffie-Hellman key exchange over
/// Z/23Z (a small prime field) using modular exponentiation.

#include <iomanip>
#include <iostream>

#include <arithkit/modint.hpp>

using arithkit::ModInt;

/// Modular exponentiation by repeated squaring.
template <unsigned N>
constexpr ModInt<N>
power(ModInt<N> base, unsigned exp) {
  auto result = ModInt<N>(1);
  while (exp > 0) {
    if (exp % 2 == 1) result = result * base;
    base = base * base;
    exp /= 2;
  }
  return result;
}

int main() {
  constexpr unsigned P = 23;
  using F = ModInt<P>;

  std::cout << "=== Modular arithmetic in Z/" << P << "Z ===" << '\n';

  // Basic arithmetic
  auto a = F(17);
  auto b = F(19);
  std::cout << '\n' << "a = " << a << '\n';
  std::cout << "b = " << b << '\n';
  std::cout << "a + b = " << (a + b) << '\n';
  std::cout << "a * b = " << (a * b) << '\n';
  std::cout << "a / b = " << (a / b) << '\n';

  // Verify: (a/b) * b == a
  std::cout << "(a/b)*b = " << ((a / b) * b) << "  (should be " << a
            << ")" << '\n';

  // Modular exponentiation
  auto g = F(5);
  std::cout << '\n' << "Generator g = " << g << '\n';
  for (unsigned i = 0; i <= 5; ++i) {
    std::cout << "g^" << i << " = " << power(g, i) << '\n';
  }

  // Toy Diffie-Hellman key exchange
  std::cout << '\n' << "=== Toy Diffie-Hellman ===" << '\n';
  unsigned alice_secret = 6;
  unsigned bob_secret = 15;

  auto alice_public = power(g, alice_secret);
  auto bob_public = power(g, bob_secret);
  std::cout << "Alice secret = " << alice_secret
            << ", public = " << alice_public << '\n';
  std::cout << "Bob   secret = " << bob_secret
            << ", public = " << bob_public << '\n';

  auto alice_shared = power(bob_public, alice_secret);
  auto bob_shared = power(alice_public, bob_secret);
  std::cout << "Alice computes shared key: " << alice_shared << '\n';
  std::cout << "Bob   computes shared key: " << bob_shared << '\n';
  std::cout << "Keys match: " << std::boolalpha
            << (alice_shared == bob_shared) << '\n';

  return 0;
}
