#pragma once

/// @file fixed_point.hpp
/// @brief Fixed-point arithmetic with compile-time bit widths.
///
/// FixedPoint<IntBits,FracBits> stores values as multiples of
/// 2^(-FracBits) in an int64_t.  Total bits = IntBits + FracBits.
///
/// Models approximate OrderedField (like double — rounding occurs
/// but algebraic laws hold approximately).

#include <compare>
#include <cstdint>
#include <ostream>

#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  namespace detail {

    /// Portable 128-bit multiply: returns (a * b) >> shift.
    /// Splits into 32-bit halves for -pedantic compliance (no __int128).
    constexpr int64_t
    mul_fixed(int64_t a, int64_t b, unsigned shift) {
      // Handle sign separately for unsigned decomposition.
      bool negative = (a < 0) != (b < 0);
      uint64_t ua =
        a < 0 ? static_cast<uint64_t>(-a) : static_cast<uint64_t>(a);
      uint64_t ub =
        b < 0 ? static_cast<uint64_t>(-b) : static_cast<uint64_t>(b);

      // Split into 32-bit halves.
      uint64_t a_hi = ua >> 32;
      uint64_t a_lo = ua & 0xFFFFFFFFULL;
      uint64_t b_hi = ub >> 32;
      uint64_t b_lo = ub & 0xFFFFFFFFULL;

      // Four partial products.
      uint64_t ll = a_lo * b_lo;
      uint64_t lh = a_lo * b_hi;
      uint64_t hl = a_hi * b_lo;
      uint64_t hh = a_hi * b_hi;

      // Accumulate into 128-bit result (hi:lo).
      uint64_t mid = lh + (ll >> 32);
      uint64_t mid2 = (mid & 0xFFFFFFFFULL) + hl;

      uint64_t lo = (mid2 << 32) | (ll & 0xFFFFFFFFULL);
      uint64_t hi = hh + (lh >> 32) + (mid2 >> 32);

      // Right-shift the 128-bit result by 'shift' bits.
      uint64_t result;
      if (shift == 0) {
        result = lo;
      } else if (shift < 64) {
        result = (lo >> shift) | (hi << (64 - shift));
      } else {
        result = hi >> (shift - 64);
      }

      return negative ? -static_cast<int64_t>(result)
                      : static_cast<int64_t>(result);
    }

  } // namespace detail

  template <unsigned IntBits, unsigned FracBits>
    requires(IntBits + FracBits <= 64 && FracBits > 0)
  class FixedPoint {
  public:
    static constexpr unsigned total_bits = IntBits + FracBits;
    static constexpr int64_t scale = int64_t{1} << FracBits;

    // --- Construction ---

    constexpr FixedPoint()
        : raw_(0) {}

    constexpr explicit FixedPoint(double val)
        : raw_(static_cast<int64_t>(val * static_cast<double>(scale))) {}

    // --- Conversion ---

    [[nodiscard]] constexpr double
    to_double() const {
      return static_cast<double>(raw_) / static_cast<double>(scale);
    }

    // --- Comparison ---

    friend constexpr bool
    operator==(const FixedPoint& a, const FixedPoint& b) {
      return a.raw_ == b.raw_;
    }

    friend constexpr auto
    operator<=>(const FixedPoint& a, const FixedPoint& b) {
      return a.raw_ <=> b.raw_;
    }

    // --- Unary ---

    constexpr FixedPoint
    operator-() const {
      FixedPoint result;
      result.raw_ = -raw_;
      return result;
    }

    constexpr FixedPoint
    operator+() const {
      return *this;
    }

    // --- Addition ---

    friend constexpr FixedPoint
    operator+(const FixedPoint& a, const FixedPoint& b) {
      FixedPoint result;
      result.raw_ = a.raw_ + b.raw_;
      return result;
    }

    // --- Subtraction ---

    friend constexpr FixedPoint
    operator-(const FixedPoint& a, const FixedPoint& b) {
      FixedPoint result;
      result.raw_ = a.raw_ - b.raw_;
      return result;
    }

    // --- Multiplication ---

    friend constexpr FixedPoint
    operator*(const FixedPoint& a, const FixedPoint& b) {
      FixedPoint result;
      result.raw_ = detail::mul_fixed(a.raw_, b.raw_, FracBits);
      return result;
    }

    // --- Division ---

    friend constexpr FixedPoint
    operator/(const FixedPoint& a, const FixedPoint& b) {
      FixedPoint result;
      // Shift dividend left by FracBits before dividing.
      // Use mul_fixed infrastructure for the left shift.
      result.raw_ = (a.raw_ * scale) / b.raw_;
      return result;
    }

    // --- Compound assignment ---

    FixedPoint&
    operator+=(const FixedPoint& other) {
      return *this = *this + other;
    }
    FixedPoint&
    operator-=(const FixedPoint& other) {
      return *this = *this - other;
    }
    FixedPoint&
    operator*=(const FixedPoint& other) {
      return *this = *this * other;
    }
    FixedPoint&
    operator/=(const FixedPoint& other) {
      return *this = *this / other;
    }

    // --- ADL-visible helpers for approximate law tests ---

    friend bool
    isfinite(const FixedPoint&) {
      return true; // Fixed-point values are always finite.
    }

    friend FixedPoint
    abs(const FixedPoint& v) {
      FixedPoint result;
      result.raw_ = v.raw_ < 0 ? -v.raw_ : v.raw_;
      return result;
    }

    // --- Stream output ---

    friend std::ostream&
    operator<<(std::ostream& os, const FixedPoint& v) {
      os << v.to_double();
      return os;
    }

  private:
    int64_t raw_;
  };

  // --- identity_element specializations ---

  template <unsigned IntBits, unsigned FracBits>
    requires(IntBits + FracBits <= 64 && FracBits > 0)
  struct identity_element<FixedPoint<IntBits, FracBits>, additive_tag> {
    static constexpr FixedPoint<IntBits, FracBits>
    make() {
      return FixedPoint<IntBits, FracBits>();
    }
  };

  template <unsigned IntBits, unsigned FracBits>
    requires(IntBits + FracBits <= 64 && FracBits > 0)
  struct identity_element<FixedPoint<IntBits, FracBits>, multiplicative_tag> {
    static constexpr FixedPoint<IntBits, FracBits>
    make() {
      return FixedPoint<IntBits, FracBits>(1.0);
    }
  };

} // namespace arithkit
