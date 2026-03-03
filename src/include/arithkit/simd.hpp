#pragma once

/// @file simd.hpp
/// @brief SIMD-like wrapper: N-wide component-wise operations.
///
/// Represents N values of type T with component-wise arithmetic.
/// Models the same algebraic concept as T (e.g., Simd<double,4>
/// models OrderedField).  Plain array implementation that benefits
/// from auto-vectorization.

#include <array>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <ostream>
#include <utility>

#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  template <typename T, std::size_t N>
  class Simd {
  public:
    // --- Construction ---

    constexpr Simd() {
      auto zero = identity_v<T, additive_tag>();
      data_.fill(zero);
    }

    constexpr explicit Simd(T scalar) { data_.fill(scalar); }

    constexpr explicit Simd(std::array<T, N> data)
        : data_(std::move(data)) {}

    /// Construct from exactly N individual values, one per lane.
    /// Requires sizeof...(Ts) == N; constrained away for N == 1 to avoid
    /// ambiguity with the scalar broadcast constructor.
    template <std::same_as<T>... Ts>
      requires(sizeof...(Ts) == N && sizeof...(Ts) >= 2)
    constexpr explicit Simd(Ts... values)
        : data_{values...} {}

    // --- Accessors ---

    [[nodiscard]] constexpr const T&
    operator[](std::size_t i) const {
      return data_[i];
    }

    [[nodiscard]] constexpr T&
    operator[](std::size_t i) {
      return data_[i];
    }

    // --- Comparison ---

    friend constexpr bool
    operator==(const Simd& a, const Simd& b) {
      return a.data_ == b.data_;
    }

    friend constexpr auto
    operator<=>(const Simd& a, const Simd& b) {
      return a.data_ <=> b.data_;
    }

    // --- Unary ---

    constexpr Simd
    operator-() const {
      Simd result;
      for (std::size_t i = 0; i < N; ++i) {
        result.data_[i] = -data_[i];
      }
      return result;
    }

    constexpr Simd
    operator+() const {
      return *this;
    }

    // --- Addition ---

    friend constexpr Simd
    operator+(const Simd& a, const Simd& b) {
      Simd result;
      for (std::size_t i = 0; i < N; ++i) {
        result.data_[i] = a.data_[i] + b.data_[i];
      }
      return result;
    }

    // --- Subtraction ---

    friend constexpr Simd
    operator-(const Simd& a, const Simd& b) {
      Simd result;
      for (std::size_t i = 0; i < N; ++i) {
        result.data_[i] = a.data_[i] - b.data_[i];
      }
      return result;
    }

    // --- Multiplication ---

    friend constexpr Simd
    operator*(const Simd& a, const Simd& b) {
      Simd result;
      for (std::size_t i = 0; i < N; ++i) {
        result.data_[i] = a.data_[i] * b.data_[i];
      }
      return result;
    }

    // --- Division ---

    friend constexpr Simd
    operator/(const Simd& a, const Simd& b) {
      Simd result;
      for (std::size_t i = 0; i < N; ++i) {
        result.data_[i] = a.data_[i] / b.data_[i];
      }
      return result;
    }

    // --- Compound assignment ---

    Simd&
    operator+=(const Simd& other) {
      return *this = *this + other;
    }
    Simd&
    operator-=(const Simd& other) {
      return *this = *this - other;
    }
    Simd&
    operator*=(const Simd& other) {
      return *this = *this * other;
    }
    Simd&
    operator/=(const Simd& other) {
      return *this = *this / other;
    }

    // --- ADL-visible helpers for approximate law tests ---

    friend bool
    isfinite(const Simd& s) {
      using std::isfinite;
      for (std::size_t i = 0; i < N; ++i) {
        if (!isfinite(s.data_[i])) return false;
      }
      return true;
    }

    friend Simd
    abs(const Simd& s) {
      using std::abs;
      Simd result;
      for (std::size_t i = 0; i < N; ++i) {
        result.data_[i] = abs(s.data_[i]);
      }
      return result;
    }

    // --- Stream output ---

    friend std::ostream&
    operator<<(std::ostream& os, const Simd& s) {
      os << "(";
      for (std::size_t i = 0; i < N; ++i) {
        if (i > 0) os << ", ";
        os << s.data_[i];
      }
      os << ")";
      return os;
    }

  private:
    std::array<T, N> data_;
  };

  // --- identity_element specializations ---

  template <typename T, std::size_t N>
  struct identity_element<Simd<T, N>, additive_tag> {
    static constexpr Simd<T, N>
    make() {
      return Simd<T, N>();
    }
  };

  template <typename T, std::size_t N>
  struct identity_element<Simd<T, N>, multiplicative_tag> {
    static constexpr Simd<T, N>
    make() {
      return Simd<T, N>(identity_v<T, multiplicative_tag>());
    }
  };

} // namespace arithkit
