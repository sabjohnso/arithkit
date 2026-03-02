#pragma once

/// @file arbitrary_float.hpp
/// @brief Arbitrary-precision floating-point number.
///
/// Represents a value as mantissa * 10^exponent where mantissa is a
/// BigInt.  Precision is controlled by a configurable number of
/// significant digits.  Models OrderedField (approximately — laws hold
/// to working precision).

#include <algorithm>
#include <cmath>
#include <compare>
#include <ostream>
#include <string>

#include <arithkit/bigint.hpp>
#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  class ArbitraryFloat {
  public:
    static constexpr int default_precision = 34;

    // --- Construction ---

    ArbitraryFloat()
        : mantissa_(0)
        , exponent_(0)
        , precision_(default_precision) {}

    explicit ArbitraryFloat(long long val)
        : mantissa_(val)
        , exponent_(0)
        , precision_(default_precision) {
      normalize();
    }

    ArbitraryFloat(long long mantissa, int exponent)
        : mantissa_(mantissa)
        , exponent_(exponent)
        , precision_(default_precision) {
      normalize();
    }

    ArbitraryFloat(BigInt mantissa, int exponent, int prec = default_precision)
        : mantissa_(std::move(mantissa))
        , exponent_(exponent)
        , precision_(prec) {
      normalize();
    }

    // --- Comparison ---

    friend bool
    operator==(const ArbitraryFloat& a, const ArbitraryFloat& b) {
      auto [aa, bb] = align(a, b);
      return aa.mantissa_ == bb.mantissa_;
    }

    friend std::strong_ordering
    operator<=>(const ArbitraryFloat& a, const ArbitraryFloat& b) {
      auto [aa, bb] = align(a, b);
      return aa.mantissa_ <=> bb.mantissa_;
    }

    // --- Unary ---

    ArbitraryFloat
    operator-() const {
      return ArbitraryFloat(-mantissa_, exponent_, precision_);
    }

    ArbitraryFloat
    operator+() const {
      return *this;
    }

    // --- Addition ---

    friend ArbitraryFloat
    operator+(const ArbitraryFloat& a, const ArbitraryFloat& b) {
      auto [aa, bb] = align(a, b);
      int prec = std::max(a.precision_, b.precision_);
      return ArbitraryFloat(aa.mantissa_ + bb.mantissa_, aa.exponent_, prec);
    }

    // --- Subtraction ---

    friend ArbitraryFloat
    operator-(const ArbitraryFloat& a, const ArbitraryFloat& b) {
      auto [aa, bb] = align(a, b);
      int prec = std::max(a.precision_, b.precision_);
      return ArbitraryFloat(aa.mantissa_ - bb.mantissa_, aa.exponent_, prec);
    }

    // --- Multiplication ---

    friend ArbitraryFloat
    operator*(const ArbitraryFloat& a, const ArbitraryFloat& b) {
      int prec = std::max(a.precision_, b.precision_);
      ArbitraryFloat result(
        a.mantissa_ * b.mantissa_, a.exponent_ + b.exponent_, prec);
      result.round_to_precision();
      return result;
    }

    // --- Division ---

    friend ArbitraryFloat
    operator/(const ArbitraryFloat& a, const ArbitraryFloat& b) {
      int prec = std::max(a.precision_, b.precision_);

      // Scale numerator up by 10^prec to get enough digits
      BigInt scaled = a.mantissa_;
      int scale_exp = 0;
      for (int i = 0; i < prec; ++i) {
        scaled = scaled * BigInt(10);
        scale_exp++;
      }

      BigInt quotient = scaled / b.mantissa_;
      int new_exp = a.exponent_ - b.exponent_ - scale_exp;

      ArbitraryFloat result(std::move(quotient), new_exp, prec);
      result.round_to_precision();
      return result;
    }

    // --- Compound assignment ---

    ArbitraryFloat&
    operator+=(const ArbitraryFloat& other) {
      return *this = *this + other;
    }
    ArbitraryFloat&
    operator-=(const ArbitraryFloat& other) {
      return *this = *this - other;
    }
    ArbitraryFloat&
    operator*=(const ArbitraryFloat& other) {
      return *this = *this * other;
    }
    ArbitraryFloat&
    operator/=(const ArbitraryFloat& other) {
      return *this = *this / other;
    }

    // --- Queries ---

    [[nodiscard]] bool
    is_zero() const {
      return mantissa_.is_zero();
    }

    // --- For std::isfinite / std::abs compatibility ---

    friend bool
    isfinite(const ArbitraryFloat&) {
      return true; // always finite
    }

    friend ArbitraryFloat
    abs(const ArbitraryFloat& a) {
      if (a.mantissa_.is_negative())
        return ArbitraryFloat(-a.mantissa_, a.exponent_, a.precision_);
      return a;
    }

    // --- Stream output ---

    friend std::ostream&
    operator<<(std::ostream& os, const ArbitraryFloat& f) {
      os << to_string(f.mantissa_) << "e" << f.exponent_;
      return os;
    }

  private:
    BigInt mantissa_;
    int exponent_;
    int precision_; // significant decimal digits

    // Raw constructor: no normalization. For internal use.
    struct raw_tag {};
    ArbitraryFloat(BigInt m, int e, int p, raw_tag)
        : mantissa_(std::move(m))
        , exponent_(e)
        , precision_(p) {}

    void
    normalize() {
      // Remove trailing zeros from mantissa, adjusting exponent
      if (mantissa_.is_zero()) {
        exponent_ = 0;
        return;
      }
      BigInt ten(10);
      BigInt zero_val(0);
      while (mantissa_ % ten == zero_val) {
        mantissa_ = mantissa_ / ten;
        exponent_++;
      }
    }

    void
    round_to_precision() {
      if (mantissa_.is_zero()) return;

      // Count digits of mantissa
      BigInt abs_m = abs(mantissa_);
      int digits = 0;
      BigInt tmp = abs_m;
      BigInt ten(10);
      while (!tmp.is_zero()) {
        tmp = tmp / ten;
        digits++;
      }

      // If more digits than precision, truncate
      int excess = digits - precision_;
      if (excess > 0) {
        for (int i = 0; i < excess; ++i) {
          mantissa_ = mantissa_ / ten;
          exponent_++;
        }
      }

      normalize();
    }

    // Align two ArbitraryFloats to the same exponent.
    // Uses raw_tag to avoid re-normalization.
    static std::pair<ArbitraryFloat, ArbitraryFloat>
    align(const ArbitraryFloat& a, const ArbitraryFloat& b) {
      if (a.exponent_ == b.exponent_) return {a, b};

      BigInt ten(10);
      if (a.exponent_ < b.exponent_) {
        int diff = b.exponent_ - a.exponent_;
        BigInt scaled = b.mantissa_;
        for (int i = 0; i < diff; ++i)
          scaled = scaled * ten;
        return {
          a, ArbitraryFloat(scaled, a.exponent_, b.precision_, raw_tag{})};
      } else {
        int diff = a.exponent_ - b.exponent_;
        BigInt scaled = a.mantissa_;
        for (int i = 0; i < diff; ++i)
          scaled = scaled * ten;
        return {
          ArbitraryFloat(scaled, b.exponent_, a.precision_, raw_tag{}), b};
      }
    }
  };

  // --- identity_element specializations ---

  template <>
  struct identity_element<ArbitraryFloat, additive_tag> {
    static ArbitraryFloat
    make() {
      return ArbitraryFloat(0);
    }
  };

  template <>
  struct identity_element<ArbitraryFloat, multiplicative_tag> {
    static ArbitraryFloat
    make() {
      return ArbitraryFloat(1);
    }
  };

} // namespace arithkit
