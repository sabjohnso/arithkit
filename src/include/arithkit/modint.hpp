#pragma once

/// @file modint.hpp
/// @brief Integers modulo a compile-time constant N.
///
/// ModInt<N> represents Z/NZ — the ring of integers modulo N.
/// Models CommutativeRing for all N >= 2.
/// Models Field when N is prime (operator/ enabled via requires).

#include <ostream>
#include <utility>

#include <arithkit/concepts/commutative_ring.hpp>
#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  namespace detail {

    /// Trial-division primality test — compile-time only, no perf concern.
    constexpr bool
    is_prime(unsigned n) {
      if (n < 2) return false;
      if (n < 4) return true;
      if (n % 2 == 0 || n % 3 == 0) return false;
      for (unsigned i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
      }
      return true;
    }

    /// Extended Euclidean algorithm: returns (gcd, x) where a*x ≡ gcd (mod m).
    constexpr int
    mod_inverse(int a, int m) {
      int g = m, x = 0, y = 1;
      int a0 = a;
      while (a0 != 0) {
        int q = g / a0;
        int t = g - q * a0;
        g = a0;
        a0 = t;
        t = x - q * y;
        x = y;
        y = t;
      }
      // x is the inverse; normalize to [0, m)
      return (x % m + m) % m;
    }

  } // namespace detail

  template <unsigned N>
    requires(N >= 2)
  class ModInt {
  public:
    // --- Construction ---

    constexpr ModInt()
        : val_(0) {}

    constexpr explicit ModInt(int v)
        : val_(
            ((v % static_cast<int>(N)) + static_cast<int>(N)) %
            static_cast<int>(N)) {}

    // --- Accessor ---

    [[nodiscard]] constexpr int
    value() const {
      return val_;
    }

    // --- Comparison ---

    friend constexpr bool
    operator==(const ModInt& a, const ModInt& b) {
      return a.val_ == b.val_;
    }

    // --- Unary ---

    constexpr ModInt
    operator-() const {
      return ModInt(val_ == 0 ? 0 : static_cast<int>(N) - val_);
    }

    constexpr ModInt
    operator+() const {
      return *this;
    }

    // --- Addition ---

    friend constexpr ModInt
    operator+(const ModInt& a, const ModInt& b) {
      return ModInt((a.val_ + b.val_) % static_cast<int>(N));
    }

    // --- Subtraction ---

    friend constexpr ModInt
    operator-(const ModInt& a, const ModInt& b) {
      return a + (-b);
    }

    // --- Multiplication ---

    friend constexpr ModInt
    operator*(const ModInt& a, const ModInt& b) {
      return ModInt((a.val_ * b.val_) % static_cast<int>(N));
    }

    // --- Division (prime moduli only) ---

    friend constexpr ModInt
    operator/(const ModInt& a, const ModInt& b)
      requires(detail::is_prime(N))
    {
      return a * ModInt(detail::mod_inverse(b.val_, N));
    }

    // --- Mixed int operators ---

    friend constexpr ModInt
    operator+(const ModInt& a, int b) {
      return a + ModInt(b);
    }
    friend constexpr ModInt
    operator+(int a, const ModInt& b) {
      return ModInt(a) + b;
    }
    friend constexpr ModInt
    operator-(const ModInt& a, int b) {
      return a - ModInt(b);
    }
    friend constexpr ModInt
    operator-(int a, const ModInt& b) {
      return ModInt(a) - b;
    }
    friend constexpr ModInt
    operator*(const ModInt& a, int b) {
      return a * ModInt(b);
    }
    friend constexpr ModInt
    operator*(int a, const ModInt& b) {
      return ModInt(a) * b;
    }

    // --- Compound assignment ---

    ModInt&
    operator+=(const ModInt& other) {
      return *this = *this + other;
    }
    ModInt&
    operator-=(const ModInt& other) {
      return *this = *this - other;
    }
    ModInt&
    operator*=(const ModInt& other) {
      return *this = *this * other;
    }
    ModInt&
    operator+=(int t) {
      return *this = *this + t;
    }
    ModInt&
    operator-=(int t) {
      return *this = *this - t;
    }
    ModInt&
    operator*=(int t) {
      return *this = *this * t;
    }

    // --- Stream output ---

    friend std::ostream&
    operator<<(std::ostream& os, const ModInt& m) {
      os << m.val_ << " (mod " << N << ")";
      return os;
    }

  private:
    int val_;

    // Private constructor from already-reduced value (no modular reduction).
    struct reduced_tag {};
    constexpr ModInt(int v, reduced_tag)
        : val_(v) {}
  };

  // --- identity_element specializations ---

  template <unsigned N>
    requires(N >= 2)
  struct identity_element<ModInt<N>, additive_tag> {
    static constexpr ModInt<N>
    make() {
      return ModInt<N>();
    }
  };

  template <unsigned N>
    requires(N >= 2)
  struct identity_element<ModInt<N>, multiplicative_tag> {
    static constexpr ModInt<N>
    make() {
      return ModInt<N>(1);
    }
  };

} // namespace arithkit
