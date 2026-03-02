#pragma once

/// @file grassmann.hpp
/// @brief Exterior (Grassmann) algebra ∧(Tⁿ) over any Field.
///
/// The algebra has 2^n dimensions.  Basis elements are indexed by
/// bitmasks representing subsets of {e₁,...,eₙ}.  The wedge product
/// is anticommutative: eᵢ∧eⱼ = −eⱼ∧eᵢ, and eᵢ∧eᵢ = 0.
///
/// Models Ring.

#include <array>
#include <cstddef>
#include <ostream>
#include <utility>

#include <arithkit/concepts/field.hpp>
#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  namespace grassmann_detail {

    /// Count transpositions needed to move bits of b past bits of a.
    constexpr int
    count_swaps(unsigned a, unsigned b) {
      int swaps = 0;
      while (b) {
        unsigned lowest = b & (~b + 1);
        // Count bits in a above this position
        int above = 0;
        unsigned tmp = a & ~(lowest - 1) & ~lowest;
        while (tmp) {
          above += static_cast<int>(tmp & 1u);
          tmp >>= 1;
        }
        swaps += above;
        b &= b - 1;
      }
      return swaps;
    }

    /// Compute the sign and resulting basis index for e_A ∧ e_B.
    /// If A and B share any generators (a & b != 0), the product is 0.
    /// Otherwise, sign = (-1)^(transpositions), result = a ^ b.
    constexpr std::pair<int, unsigned>
    wedge_product(unsigned a, unsigned b) {
      // If any generator appears in both, result is 0
      if (a & b) return {0, 0};

      int sign = (count_swaps(a, b) % 2 == 0) ? 1 : -1;
      return {sign, a ^ b};
    }

  } // namespace grassmann_detail

  template <Field T, unsigned N>
  class Grassmann {
    static constexpr std::size_t DIM = std::size_t(1) << N;

  public:
    // --- Construction ---

    Grassmann() {
      auto zero = identity_v<T, additive_tag>();
      coeffs_.fill(zero);
    }

    explicit Grassmann(T scalar) {
      auto zero = identity_v<T, additive_tag>();
      coeffs_.fill(zero);
      coeffs_[0] = std::move(scalar);
    }

    explicit Grassmann(std::array<T, DIM> coeffs)
        : coeffs_(std::move(coeffs)) {}

    // --- Accessors ---

    [[nodiscard]] const T&
    operator[](std::size_t i) const {
      return coeffs_[i];
    }

    [[nodiscard]] T&
    operator[](std::size_t i) {
      return coeffs_[i];
    }

    [[nodiscard]] static constexpr std::size_t
    dim() {
      return DIM;
    }

    // --- Comparison ---

    friend bool
    operator==(const Grassmann& a, const Grassmann& b) {
      return a.coeffs_ == b.coeffs_;
    }

    // --- Unary ---

    Grassmann
    operator-() const {
      Grassmann result;
      for (std::size_t i = 0; i < DIM; ++i) {
        result.coeffs_[i] = -coeffs_[i];
      }
      return result;
    }

    Grassmann
    operator+() const {
      return *this;
    }

    // --- Addition ---

    friend Grassmann
    operator+(const Grassmann& a, const Grassmann& b) {
      Grassmann result;
      for (std::size_t i = 0; i < DIM; ++i) {
        result.coeffs_[i] = a.coeffs_[i] + b.coeffs_[i];
      }
      return result;
    }

    // --- Subtraction ---

    friend Grassmann
    operator-(const Grassmann& a, const Grassmann& b) {
      Grassmann result;
      for (std::size_t i = 0; i < DIM; ++i) {
        result.coeffs_[i] = a.coeffs_[i] - b.coeffs_[i];
      }
      return result;
    }

    // --- Multiplication (wedge product) ---

    friend Grassmann
    operator*(const Grassmann& a, const Grassmann& b) {
      Grassmann result;
      for (unsigned i = 0; i < DIM; ++i) {
        for (unsigned j = 0; j < DIM; ++j) {
          auto [sign, idx] = grassmann_detail::wedge_product(i, j);
          if (sign == 0) continue;
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

    Grassmann&
    operator+=(const Grassmann& other) {
      return *this = *this + other;
    }
    Grassmann&
    operator-=(const Grassmann& other) {
      return *this = *this - other;
    }
    Grassmann&
    operator*=(const Grassmann& other) {
      return *this = *this * other;
    }

    // --- Stream output ---

    friend std::ostream&
    operator<<(std::ostream& os, const Grassmann& g) {
      os << "(";
      for (std::size_t i = 0; i < DIM; ++i) {
        if (i > 0) os << " + ";
        os << g.coeffs_[i] << "e" << i;
      }
      os << ")";
      return os;
    }

  private:
    std::array<T, DIM> coeffs_;
  };

  // --- identity_element specializations ---

  template <Field T, unsigned N>
  struct identity_element<Grassmann<T, N>, additive_tag> {
    static Grassmann<T, N>
    make() {
      return Grassmann<T, N>();
    }
  };

  template <Field T, unsigned N>
  struct identity_element<Grassmann<T, N>, multiplicative_tag> {
    static Grassmann<T, N>
    make() {
      return Grassmann<T, N>(identity_v<T, multiplicative_tag>());
    }
  };

} // namespace arithkit
