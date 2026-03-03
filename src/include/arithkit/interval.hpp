#pragma once

/// @file interval.hpp
/// @brief Closed interval arithmetic over any OrderedField.
///
/// Represents [lo, hi] with outward-rounding semantics.  Models
/// IntervalArithmetic but NOT Ring (subdistributivity, dependency
/// problem).
///
/// Key property: if x ∈ X and y ∈ Y, then x○y ∈ X○Y for ○ ∈ {+,-,*,/}.

#include <algorithm>
#include <ostream>
#include <utility>

#include <arithkit/concepts/ordered_field.hpp>
#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  template <OrderedField T>
  class Interval {
  public:
    // --- Construction ---

    constexpr Interval()
        : lo_(identity_v<T, additive_tag>())
        , hi_(identity_v<T, additive_tag>()) {}

    constexpr explicit Interval(T point)
        : lo_(point)
        , hi_(std::move(point)) {}

    constexpr Interval(T lo, T hi)
        : lo_(std::min(lo, hi))
        , hi_(std::max(std::move(lo), std::move(hi))) {}

    // --- Accessors ---

    [[nodiscard]] constexpr const T&
    lo() const {
      return lo_;
    }

    [[nodiscard]] constexpr const T&
    hi() const {
      return hi_;
    }

    [[nodiscard]] constexpr T
    width() const {
      return hi_ - lo_;
    }

    [[nodiscard]] constexpr T
    midpoint() const {
      auto two = identity_v<T, multiplicative_tag>() +
                 identity_v<T, multiplicative_tag>();
      return (lo_ + hi_) / two;
    }

    // --- Queries ---

    [[nodiscard]] constexpr bool
    contains(const T& x) const {
      return lo_ <= x && x <= hi_;
    }

    [[nodiscard]] constexpr bool
    is_subset_of(const Interval& other) const {
      return other.lo_ <= lo_ && hi_ <= other.hi_;
    }

    // --- Comparison ---

    friend constexpr bool
    operator==(const Interval& a, const Interval& b) {
      return a.lo_ == b.lo_ && a.hi_ == b.hi_;
    }

    // --- Unary ---

    constexpr Interval
    operator-() const {
      return Interval(-hi_, -lo_);
    }

    constexpr Interval
    operator+() const {
      return *this;
    }

    // --- Addition ---
    // [a,b] + [c,d] = [a+c, b+d]

    friend constexpr Interval
    operator+(const Interval& a, const Interval& b) {
      return Interval(a.lo_ + b.lo_, a.hi_ + b.hi_);
    }

    // --- Subtraction ---
    // [a,b] - [c,d] = [a-d, b-c]

    friend constexpr Interval
    operator-(const Interval& a, const Interval& b) {
      return Interval(a.lo_ - b.hi_, a.hi_ - b.lo_);
    }

    // --- Multiplication ---
    // [a,b] * [c,d] = [min(ac,ad,bc,bd), max(ac,ad,bc,bd)]

    friend constexpr Interval
    operator*(const Interval& a, const Interval& b) {
      auto ac = a.lo_ * b.lo_;
      auto ad = a.lo_ * b.hi_;
      auto bc = a.hi_ * b.lo_;
      auto bd = a.hi_ * b.hi_;
      return Interval(std::min({ac, ad, bc, bd}), std::max({ac, ad, bc, bd}));
    }

    // --- Division ---
    // [a,b] / [c,d] where 0 not in [c,d]
    // = [a,b] * [1/d, 1/c]

    friend constexpr Interval
    operator/(const Interval& a, const Interval& b) {
      auto one = identity_v<T, multiplicative_tag>();
      auto inv = Interval(one / b.hi_, one / b.lo_);
      return a * inv;
    }

    // --- Compound assignment ---

    Interval&
    operator+=(const Interval& other) {
      return *this = *this + other;
    }
    Interval&
    operator-=(const Interval& other) {
      return *this = *this - other;
    }
    Interval&
    operator*=(const Interval& other) {
      return *this = *this * other;
    }
    Interval&
    operator/=(const Interval& other) {
      return *this = *this / other;
    }

    // --- Stream output ---

    friend std::ostream&
    operator<<(std::ostream& os, const Interval& iv) {
      os << "[" << iv.lo_ << ", " << iv.hi_ << "]";
      return os;
    }

  private:
    T lo_;
    T hi_;
  };

  // --- identity_element specializations ---

  template <OrderedField T>
  struct identity_element<Interval<T>, additive_tag> {
    static constexpr Interval<T>
    make() {
      return Interval<T>();
    }
  };

  template <OrderedField T>
  struct identity_element<Interval<T>, multiplicative_tag> {
    static constexpr Interval<T>
    make() {
      return Interval<T>(identity_v<T, multiplicative_tag>());
    }
  };

} // namespace arithkit
