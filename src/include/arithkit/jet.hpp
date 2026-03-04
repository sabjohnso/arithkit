#pragma once

/// @file jet.hpp
/// @brief N-th order truncated Taylor series (jets).
///
/// Jet<T,N> carries a value and N derivatives, stored as Taylor
/// coefficients: coefficients[k] = f^(k)(a) / k!.  This generalizes
/// Dual<T> (which is Jet<T,1>) to higher-order automatic
/// differentiation.
///
/// Models CommutativeRing.  NOT a Field — higher-order nilpotents
/// (ε^k for k > 0) are zero divisors.

#include <algorithm>
#include <array>
#include <cstddef>
#include <ostream>

#include <arithkit/concepts/commutative_ring.hpp>
#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  template <CommutativeRing T, std::size_t N>
  class Jet {
  public:
    static constexpr std::size_t order = N;
    static constexpr std::size_t size = N + 1;

    // --- Construction ---

    constexpr Jet() { coeffs_.fill(identity_v<T, additive_tag>()); }

    constexpr explicit Jet(T scalar) {
      coeffs_.fill(identity_v<T, additive_tag>());
      coeffs_[0] = std::move(scalar);
    }

    constexpr explicit Jet(std::array<T, N + 1> c)
        : coeffs_(std::move(c)) {}

    // --- Accessors ---

    [[nodiscard]] constexpr const T&
    value() const {
      return coeffs_[0];
    }

    [[nodiscard]] constexpr const T&
    operator[](std::size_t i) const {
      return coeffs_[i];
    }

    [[nodiscard]] constexpr T&
    operator[](std::size_t i) {
      return coeffs_[i];
    }

    // --- Comparison ---

    friend constexpr bool
    operator==(const Jet& a, const Jet& b) {
      return a.coeffs_ == b.coeffs_;
    }

    // --- Unary ---

    constexpr Jet
    operator-() const {
      Jet result;
      for (std::size_t i = 0; i < size; ++i) {
        result.coeffs_[i] = -coeffs_[i];
      }
      return result;
    }

    constexpr Jet
    operator+() const {
      return *this;
    }

    // --- Addition ---

    friend constexpr Jet
    operator+(const Jet& a, const Jet& b) {
      Jet result;
      for (std::size_t i = 0; i < size; ++i) {
        result.coeffs_[i] = a.coeffs_[i] + b.coeffs_[i];
      }
      return result;
    }

    // --- Subtraction ---

    friend constexpr Jet
    operator-(const Jet& a, const Jet& b) {
      Jet result;
      for (std::size_t i = 0; i < size; ++i) {
        result.coeffs_[i] = a.coeffs_[i] - b.coeffs_[i];
      }
      return result;
    }

    // --- Multiplication (truncated Cauchy product) ---

    friend constexpr Jet
    operator*(const Jet& a, const Jet& b) {
      auto zero = identity_v<T, additive_tag>();
      Jet result;
      for (std::size_t k = 0; k < size; ++k) {
        T sum = zero;
        for (std::size_t j = 0; j <= k; ++j) {
          sum = sum + a.coeffs_[j] * b.coeffs_[k - j];
        }
        result.coeffs_[k] = sum;
      }
      return result;
    }

    // No operator/ — Jet is NOT a field (nilpotents are zero divisors).

    // --- Mixed scalar/Jet operators (T embeds as t ↦ (t,0,...,0)) ---

    friend constexpr Jet
    operator+(const Jet& z, const T& t) {
      return z + Jet(t);
    }
    friend constexpr Jet
    operator+(const T& t, const Jet& z) {
      return Jet(t) + z;
    }
    friend constexpr Jet
    operator-(const Jet& z, const T& t) {
      return z - Jet(t);
    }
    friend constexpr Jet
    operator-(const T& t, const Jet& z) {
      return Jet(t) - z;
    }
    friend constexpr Jet
    operator*(const Jet& z, const T& t) {
      return z * Jet(t);
    }
    friend constexpr Jet
    operator*(const T& t, const Jet& z) {
      return Jet(t) * z;
    }

    // --- Compound assignment ---

    Jet&
    operator+=(const Jet& other) {
      return *this = *this + other;
    }
    Jet&
    operator-=(const Jet& other) {
      return *this = *this - other;
    }
    Jet&
    operator*=(const Jet& other) {
      return *this = *this * other;
    }
    Jet&
    operator+=(const T& t) {
      return *this = *this + t;
    }
    Jet&
    operator-=(const T& t) {
      return *this = *this - t;
    }
    Jet&
    operator*=(const T& t) {
      return *this = *this * t;
    }

    // --- Stream output ---

    friend std::ostream&
    operator<<(std::ostream& os, const Jet& z) {
      os << "Jet(";
      for (std::size_t i = 0; i < size; ++i) {
        if (i > 0) os << ", ";
        os << z.coeffs_[i];
      }
      os << ")";
      return os;
    }

  private:
    std::array<T, N + 1> coeffs_;
  };

  // --- identity_element specializations ---

  template <CommutativeRing T, std::size_t N>
  struct identity_element<Jet<T, N>, additive_tag> {
    static constexpr Jet<T, N>
    make() {
      return Jet<T, N>();
    }
  };

  template <CommutativeRing T, std::size_t N>
  struct identity_element<Jet<T, N>, multiplicative_tag> {
    static constexpr Jet<T, N>
    make() {
      return Jet<T, N>(identity_v<T, multiplicative_tag>());
    }
  };

} // namespace arithkit
