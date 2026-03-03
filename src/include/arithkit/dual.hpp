#pragma once

/// @file dual.hpp
/// @brief Dual number parameterized over any CommutativeRing.
///
/// Represents a + bε where ε² = 0.  Enables forward-mode automatic
/// differentiation: f(a+ε) evaluates to f(a) + f'(a)ε.
///
/// Models CommutativeRing.  NOT a Field — ε is a zero divisor
/// (ε² = 0 means ε has no multiplicative inverse).

#include <ostream>
#include <utility>

#include <arithkit/concepts/commutative_ring.hpp>
#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  template <CommutativeRing T>
  class Dual {
  public:
    // --- Construction ---

    constexpr Dual()
        : re_(identity_v<T, additive_tag>())
        , du_(identity_v<T, additive_tag>()) {}

    constexpr explicit Dual(T re)
        : re_(std::move(re))
        , du_(identity_v<T, additive_tag>()) {}

    constexpr Dual(T re, T du)
        : re_(std::move(re))
        , du_(std::move(du)) {}

    // --- Accessors ---

    [[nodiscard]] constexpr const T&
    real() const {
      return re_;
    }

    [[nodiscard]] constexpr const T&
    dual() const {
      return du_;
    }

    // --- Comparison ---

    friend constexpr bool
    operator==(const Dual& a, const Dual& b) {
      return a.re_ == b.re_ && a.du_ == b.du_;
    }

    // --- Unary ---

    constexpr Dual
    operator-() const {
      return Dual(-re_, -du_);
    }

    constexpr Dual
    operator+() const {
      return *this;
    }

    // --- Addition ---

    friend constexpr Dual
    operator+(const Dual& a, const Dual& b) {
      return Dual(a.re_ + b.re_, a.du_ + b.du_);
    }

    // --- Subtraction ---

    friend constexpr Dual
    operator-(const Dual& a, const Dual& b) {
      return Dual(a.re_ - b.re_, a.du_ - b.du_);
    }

    // --- Multiplication ---
    // (a+bε)(c+dε) = ac + (ad+bc)ε  (since ε²=0)

    friend constexpr Dual
    operator*(const Dual& a, const Dual& b) {
      return Dual(a.re_ * b.re_, a.re_ * b.du_ + a.du_ * b.re_);
    }

    // No operator/ — Dual is NOT a field (ε is a zero divisor).

    // --- Mixed scalar/dual operators (T embeds as subring t ↦ (t,0)) ---

    friend constexpr Dual
    operator+(const Dual& z, const T& t) {
      return z + Dual(t);
    }
    friend constexpr Dual
    operator+(const T& t, const Dual& z) {
      return Dual(t) + z;
    }
    friend constexpr Dual
    operator-(const Dual& z, const T& t) {
      return z - Dual(t);
    }
    friend constexpr Dual
    operator-(const T& t, const Dual& z) {
      return Dual(t) - z;
    }
    friend constexpr Dual
    operator*(const Dual& z, const T& t) {
      return z * Dual(t);
    }
    friend constexpr Dual
    operator*(const T& t, const Dual& z) {
      return Dual(t) * z;
    }

    // --- Compound assignment ---

    Dual&
    operator+=(const Dual& other) {
      return *this = *this + other;
    }
    Dual&
    operator-=(const Dual& other) {
      return *this = *this - other;
    }
    Dual&
    operator*=(const Dual& other) {
      return *this = *this * other;
    }
    Dual&
    operator+=(const T& t) {
      return *this = *this + t;
    }
    Dual&
    operator-=(const T& t) {
      return *this = *this - t;
    }
    Dual&
    operator*=(const T& t) {
      return *this = *this * t;
    }

    // --- Stream output ---

    friend std::ostream&
    operator<<(std::ostream& os, const Dual& z) {
      os << "(" << z.re_ << "+" << z.du_ << "ε)";
      return os;
    }

  private:
    T re_;
    T du_;
  };

  // --- identity_element specializations ---

  template <CommutativeRing T>
  struct identity_element<Dual<T>, additive_tag> {
    static constexpr Dual<T>
    make() {
      return Dual<T>();
    }
  };

  template <CommutativeRing T>
  struct identity_element<Dual<T>, multiplicative_tag> {
    static constexpr Dual<T>
    make() {
      return Dual<T>(identity_v<T, multiplicative_tag>());
    }
  };

} // namespace arithkit
