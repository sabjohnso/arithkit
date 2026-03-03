#pragma once

/// @file clifford.hpp
/// @brief Clifford algebra Cl(p,q) over any Field.
///
/// The algebra has 2^(p+q) dimensions.  Basis elements are indexed
/// by bitmasks representing subsets of {e₁,...,e_{p+q}}.  The first p
/// generators square to +1, the next q square to −1.
///
/// Special cases:
///   Cl(0,1) ≅ Complex numbers
///   Cl(1,0) ≅ Split-complex (hyperbolic) numbers
///   Cl(0,2) ≅ Quaternions
///
/// Models Ring.

#include <array>
#include <cstddef>
#include <ostream>
#include <utility>

#include <arithkit/concepts/field.hpp>
#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  namespace clifford_detail {

    /// Number of 1-bits in a bitmask.
    constexpr int
    popcount(unsigned x) {
      int count = 0;
      while (x) {
        count += static_cast<int>(x & 1u);
        x >>= 1;
      }
      return count;
    }

    /// Count transpositions needed to move bits of b past bits of a.
    /// This gives the sign factor (-1)^swaps for the product of
    /// basis elements e_A * e_B.
    constexpr int
    count_swaps(unsigned a, unsigned b) {
      int swaps = 0;
      while (b) {
        // Find lowest set bit in b
        unsigned lowest = b & (~b + 1);
        // Count bits in a that are above this position
        // (they need to be swapped past)
        swaps += popcount(a & ~(lowest - 1) & ~lowest);
        b &= b - 1; // clear lowest bit
      }
      return swaps;
    }

    /// Compute the sign and resulting basis index for e_A * e_B
    /// in Cl(p,q).
    /// Returns {sign (+1 or -1), result_index}.
    template <unsigned P, unsigned Q>
    constexpr std::pair<int, unsigned>
    basis_product(unsigned a, unsigned b) {
      // Sign from reordering: (-1)^(number of transpositions)
      int sign = (count_swaps(a, b) % 2 == 0) ? 1 : -1;

      // Result basis element (symmetric difference / XOR)
      unsigned result = a ^ b;

      // For each common bit (a & b), that generator squares
      // to +1 (if position < P) or -1 (if position >= P)
      unsigned common = a & b;
      while (common) {
        unsigned lowest = common & (~common + 1);
        // Find bit position
        unsigned pos = 0;
        unsigned tmp = lowest;
        while (tmp >>= 1)
          pos++;
        // First P generators square to +1, next Q to -1
        if (pos >= P) { sign = -sign; }
        common &= common - 1;
      }

      return {sign, result};
    }

  } // namespace clifford_detail

  template <Field T, unsigned P, unsigned Q>
  class Clifford {
    static constexpr unsigned N = P + Q;
    static constexpr std::size_t DIM = std::size_t(1) << N;

  public:
    // --- Construction ---

    constexpr Clifford() {
      auto zero = identity_v<T, additive_tag>();
      coeffs_.fill(zero);
    }

    constexpr explicit Clifford(T scalar) {
      auto zero = identity_v<T, additive_tag>();
      coeffs_.fill(zero);
      coeffs_[0] = std::move(scalar);
    }

    constexpr explicit Clifford(std::array<T, DIM> coeffs)
        : coeffs_(std::move(coeffs)) {}

    // --- Accessors ---

    [[nodiscard]] constexpr const T&
    operator[](std::size_t i) const {
      return coeffs_[i];
    }

    [[nodiscard]] constexpr T&
    operator[](std::size_t i) {
      return coeffs_[i];
    }

    [[nodiscard]] static constexpr std::size_t
    dim() {
      return DIM;
    }

    // --- Comparison ---

    friend constexpr bool
    operator==(const Clifford& a, const Clifford& b) {
      return a.coeffs_ == b.coeffs_;
    }

    // --- Unary ---

    constexpr Clifford
    operator-() const {
      Clifford result;
      for (std::size_t i = 0; i < DIM; ++i) {
        result.coeffs_[i] = -coeffs_[i];
      }
      return result;
    }

    constexpr Clifford
    operator+() const {
      return *this;
    }

    // --- Addition ---

    friend constexpr Clifford
    operator+(const Clifford& a, const Clifford& b) {
      Clifford result;
      for (std::size_t i = 0; i < DIM; ++i) {
        result.coeffs_[i] = a.coeffs_[i] + b.coeffs_[i];
      }
      return result;
    }

    // --- Subtraction ---

    friend constexpr Clifford
    operator-(const Clifford& a, const Clifford& b) {
      Clifford result;
      for (std::size_t i = 0; i < DIM; ++i) {
        result.coeffs_[i] = a.coeffs_[i] - b.coeffs_[i];
      }
      return result;
    }

    // --- Multiplication (geometric product) ---

    friend constexpr Clifford
    operator*(const Clifford& a, const Clifford& b) {
      Clifford result;
      for (unsigned i = 0; i < DIM; ++i) {
        for (unsigned j = 0; j < DIM; ++j) {
          auto [sign, idx] = clifford_detail::basis_product<P, Q>(i, j);
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

    // --- Compound assignment ---

    Clifford&
    operator+=(const Clifford& other) {
      return *this = *this + other;
    }
    Clifford&
    operator-=(const Clifford& other) {
      return *this = *this - other;
    }
    Clifford&
    operator*=(const Clifford& other) {
      return *this = *this * other;
    }

    // --- Stream output ---

    friend std::ostream&
    operator<<(std::ostream& os, const Clifford& c) {
      os << "(";
      for (std::size_t i = 0; i < DIM; ++i) {
        if (i > 0) os << " + ";
        os << c.coeffs_[i] << "e" << i;
      }
      os << ")";
      return os;
    }

  private:
    std::array<T, DIM> coeffs_;
  };

  // --- identity_element specializations ---

  template <Field T, unsigned P, unsigned Q>
  struct identity_element<Clifford<T, P, Q>, additive_tag> {
    static constexpr Clifford<T, P, Q>
    make() {
      return Clifford<T, P, Q>();
    }
  };

  template <Field T, unsigned P, unsigned Q>
  struct identity_element<Clifford<T, P, Q>, multiplicative_tag> {
    static constexpr Clifford<T, P, Q>
    make() {
      return Clifford<T, P, Q>(identity_v<T, multiplicative_tag>());
    }
  };

} // namespace arithkit
