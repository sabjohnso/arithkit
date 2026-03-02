#pragma once

/// @file fraction.hpp
/// @brief Exact fraction (rational number) parameterized over any
///        IntegralDomain.
///
/// Models Field. Models OrderedField when the underlying type is
/// ordered. Automatically reduces to lowest terms via GCD.

#include <compare>
#include <ostream>
#include <string>
#include <utility>

#include <arithkit/concepts/integral_domain.hpp>
#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  /// Compute GCD via Euclidean algorithm.  Requires / and % on I.
  /// Returns a non-negative result.
  template <typename I>
  I
  fraction_gcd(I a, I b) {
    auto zero = identity_v<I, additive_tag>();
    if (a < zero) a = -a;
    if (b < zero) b = -b;
    while (b != zero) {
      auto r = a % b;
      a = std::move(b);
      b = std::move(r);
    }
    return a;
  }

  template <IntegralDomain I>
  class Fraction {
  public:
    // --- Construction ---

    Fraction()
        : num_(identity_v<I, additive_tag>())
        , den_(identity_v<I, multiplicative_tag>()) {}

    explicit Fraction(I n)
        : num_(std::move(n))
        , den_(identity_v<I, multiplicative_tag>()) {}

    Fraction(I n, I d)
        : num_(std::move(n))
        , den_(std::move(d)) {
      reduce();
    }

    // --- Accessors ---

    [[nodiscard]] const I&
    numerator() const {
      return num_;
    }
    [[nodiscard]] const I&
    denominator() const {
      return den_;
    }

    // --- Comparison ---

    friend bool
    operator==(const Fraction& a, const Fraction& b) {
      return a.num_ == b.num_ && a.den_ == b.den_;
    }

    friend auto
    operator<=>(const Fraction& a, const Fraction& b) {
      // a/b <=> c/d  is equivalent to  a*d <=> c*b (when
      // denominators are positive, which reduce() guarantees)
      return (a.num_ * b.den_) <=> (b.num_ * a.den_);
    }

    // --- Unary ---

    Fraction
    operator-() const {
      return Fraction(-num_, den_);
    }

    Fraction
    operator+() const {
      return *this;
    }

    // --- Arithmetic ---

    friend Fraction
    operator+(const Fraction& a, const Fraction& b) {
      return Fraction(a.num_ * b.den_ + b.num_ * a.den_, a.den_ * b.den_);
    }

    friend Fraction
    operator-(const Fraction& a, const Fraction& b) {
      return Fraction(a.num_ * b.den_ - b.num_ * a.den_, a.den_ * b.den_);
    }

    friend Fraction
    operator*(const Fraction& a, const Fraction& b) {
      return Fraction(a.num_ * b.num_, a.den_ * b.den_);
    }

    friend Fraction
    operator/(const Fraction& a, const Fraction& b) {
      return Fraction(a.num_ * b.den_, a.den_ * b.num_);
    }

    // --- Compound assignment ---

    Fraction&
    operator+=(const Fraction& other) {
      return *this = *this + other;
    }
    Fraction&
    operator-=(const Fraction& other) {
      return *this = *this - other;
    }
    Fraction&
    operator*=(const Fraction& other) {
      return *this = *this * other;
    }
    Fraction&
    operator/=(const Fraction& other) {
      return *this = *this / other;
    }

    // --- String conversion ---

    friend std::string
    to_string(const Fraction& f) {
      auto one = identity_v<I, multiplicative_tag>();
      auto num_str = stringify(f.num_);
      if (f.den_ == one) return num_str;
      return num_str + "/" + stringify(f.den_);
    }

  private:
    // ADL-friendly string conversion helper
    static std::string
    stringify(const I& val) {
      using std::to_string;
      return to_string(val);
    }

  public:
    friend std::ostream&
    operator<<(std::ostream& os, const Fraction& f) {
      return os << to_string(f);
    }

  private:
    I num_;
    I den_;

    void
    reduce() {
      auto zero = identity_v<I, additive_tag>();
      auto one = identity_v<I, multiplicative_tag>();

      if (den_ == zero) {
        // Undefined — normalize to 0/1
        num_ = zero;
        den_ = one;
        return;
      }

      // Ensure denominator is positive
      if (den_ < zero) {
        num_ = -num_;
        den_ = -den_;
      }

      // Reduce by GCD
      auto g = fraction_gcd(num_, den_);
      if (g != zero && g != one) {
        num_ = num_ / g;
        den_ = den_ / g;
      }
    }
  };

  // --- identity_element specializations ---

  template <IntegralDomain I>
  struct identity_element<Fraction<I>, additive_tag> {
    static Fraction<I>
    make() {
      return Fraction<I>(identity_v<I, additive_tag>());
    }
  };

  template <IntegralDomain I>
  struct identity_element<Fraction<I>, multiplicative_tag> {
    static Fraction<I>
    make() {
      return Fraction<I>(identity_v<I, multiplicative_tag>());
    }
  };

} // namespace arithkit
