#pragma once

/// @file octonion.hpp
/// @brief Octonion parameterized over any Field.
///
/// Represents an 8-dimensional normed division algebra:
/// a₀ + a₁e₁ + a₂e₂ + ... + a₇e₇
///
/// Multiplication is defined by the Fano plane. Every nonzero
/// octonion has a multiplicative inverse, but multiplication is
/// neither commutative nor associative. It satisfies alternativity:
///   left:        a*(a*b) == (a*a)*b
///   right:       (a*b)*b == a*(b*b)
///   flexibility: a*(b*a) == (a*b)*a
///
/// Models AlternativeDivisionAlgebra.

#include <array>
#include <cstddef>
#include <ostream>
#include <utility>

#include <arithkit/concepts/field.hpp>
#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  namespace octonion_detail {

    struct BasisProduct {
      int sign;
      unsigned index;
    };

    /// Build the 8x8 multiplication table from the Fano plane triples.
    /// Each triple (i,j,k) encodes: e_i * e_j = +e_k (cyclic).
    constexpr auto
    build_mult_table() {
      std::array<std::array<BasisProduct, 8>, 8> table{};

      // e_0 is the identity: e_0 * e_i = e_i, e_i * e_0 = e_i
      for (unsigned i = 0; i < 8; ++i) {
        table[0][i] = {1, i};
        table[i][0] = {1, i};
      }

      // e_i * e_i = -e_0 for i = 1..7
      for (unsigned i = 1; i < 8; ++i) {
        table[i][i] = {-1, 0};
      }

      // Fano plane triples: (i,j,k) means e_i * e_j = +e_k
      struct Triple {
        unsigned i, j, k;
      };
      constexpr Triple triples[] = {
        {1, 2, 3},
        {1, 4, 5},
        {1, 7, 6},
        {2, 4, 6},
        {2, 5, 7},
        {3, 4, 7},
        {3, 6, 5}};

      for (auto [i, j, k] : triples) {
        // Cyclic products
        table[i][j] = {1, k};
        table[j][k] = {1, i};
        table[k][i] = {1, j};

        // Anti-cyclic products
        table[j][i] = {-1, k};
        table[k][j] = {-1, i};
        table[i][k] = {-1, j};
      }

      return table;
    }

    inline constexpr auto mult_table = build_mult_table();

  } // namespace octonion_detail

  template <Field T>
  class Octonion {
  public:
    // --- Construction ---

    constexpr Octonion() {
      auto zero = identity_v<T, additive_tag>();
      coeffs_.fill(zero);
    }

    constexpr explicit Octonion(T scalar) {
      auto zero = identity_v<T, additive_tag>();
      coeffs_.fill(zero);
      coeffs_[0] = std::move(scalar);
    }

    constexpr explicit Octonion(std::array<T, 8> coeffs)
        : coeffs_(std::move(coeffs)) {}

    constexpr Octonion(T e0, T e1, T e2, T e3, T e4, T e5, T e6, T e7)
        : coeffs_{
            std::move(e0),
            std::move(e1),
            std::move(e2),
            std::move(e3),
            std::move(e4),
            std::move(e5),
            std::move(e6),
            std::move(e7)} {}

    // --- Accessors ---

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
    operator==(const Octonion& a, const Octonion& b) {
      return a.coeffs_ == b.coeffs_;
    }

    // No operator<=> — octonions are not ordered.

    // --- Unary ---

    constexpr Octonion
    operator-() const {
      Octonion result;
      for (std::size_t i = 0; i < 8; ++i) {
        result.coeffs_[i] = -coeffs_[i];
      }
      return result;
    }

    constexpr Octonion
    operator+() const {
      return *this;
    }

    // --- Addition ---

    friend constexpr Octonion
    operator+(const Octonion& a, const Octonion& b) {
      Octonion result;
      for (std::size_t i = 0; i < 8; ++i) {
        result.coeffs_[i] = a.coeffs_[i] + b.coeffs_[i];
      }
      return result;
    }

    // --- Subtraction ---

    friend constexpr Octonion
    operator-(const Octonion& a, const Octonion& b) {
      Octonion result;
      for (std::size_t i = 0; i < 8; ++i) {
        result.coeffs_[i] = a.coeffs_[i] - b.coeffs_[i];
      }
      return result;
    }

    // --- Multiplication (Fano plane table) ---

    friend constexpr Octonion
    operator*(const Octonion& a, const Octonion& b) {
      Octonion result;
      for (unsigned i = 0; i < 8; ++i) {
        for (unsigned j = 0; j < 8; ++j) {
          auto [sign, idx] = octonion_detail::mult_table[i][j];
          if (sign > 0) {
            result.coeffs_[idx] =
              result.coeffs_[idx] + a.coeffs_[i] * b.coeffs_[j];
          } else {
            result.coeffs_[idx] =
              result.coeffs_[idx] - a.coeffs_[i] * b.coeffs_[j];
          }
        }
      }
      return result;
    }

    // --- Division ---
    // a / b = a * b⁻¹ where b⁻¹ = conj(b) / norm(b)

    friend constexpr Octonion
    operator/(const Octonion& a, const Octonion& b) {
      auto n = norm(b);
      auto conj_b = conjugate(b);
      Octonion inv;
      for (std::size_t i = 0; i < 8; ++i) {
        inv.coeffs_[i] = conj_b.coeffs_[i] / n;
      }
      return a * inv;
    }

    // --- Compound assignment ---

    Octonion&
    operator+=(const Octonion& other) {
      return *this = *this + other;
    }
    Octonion&
    operator-=(const Octonion& other) {
      return *this = *this - other;
    }
    Octonion&
    operator*=(const Octonion& other) {
      return *this = *this * other;
    }
    Octonion&
    operator/=(const Octonion& other) {
      return *this = *this / other;
    }

    // --- Conjugate and norm ---

    friend constexpr Octonion
    conjugate(const Octonion& q) {
      Octonion result;
      result.coeffs_[0] = q.coeffs_[0];
      for (std::size_t i = 1; i < 8; ++i) {
        result.coeffs_[i] = -q.coeffs_[i];
      }
      return result;
    }

    /// Norm (squared magnitude): |q|² = Σ aᵢ²
    friend constexpr T
    norm(const Octonion& q) {
      auto result = q.coeffs_[0] * q.coeffs_[0];
      for (std::size_t i = 1; i < 8; ++i) {
        result = result + q.coeffs_[i] * q.coeffs_[i];
      }
      return result;
    }

    // --- Stream output ---

    friend std::ostream&
    operator<<(std::ostream& os, const Octonion& q) {
      os << "(";
      for (std::size_t i = 0; i < 8; ++i) {
        if (i > 0) os << "+";
        os << q.coeffs_[i] << "e" << i;
      }
      os << ")";
      return os;
    }

  private:
    std::array<T, 8> coeffs_;
  };

  // --- identity_element specializations ---

  template <Field T>
  struct identity_element<Octonion<T>, additive_tag> {
    static constexpr Octonion<T>
    make() {
      return Octonion<T>();
    }
  };

  template <Field T>
  struct identity_element<Octonion<T>, multiplicative_tag> {
    static constexpr Octonion<T>
    make() {
      return Octonion<T>(identity_v<T, multiplicative_tag>());
    }
  };

} // namespace arithkit
