#pragma once

/// @file complex.hpp
/// @brief Complex number parameterized over any Ring.
///
/// Represents a + bi where i² = −1.  Models Ring when T models Ring,
/// Field when T models Field.  Complex numbers are not ordered.

#include <ostream>
#include <utility>

#include <arithkit/concepts/field.hpp>
#include <arithkit/concepts/ring.hpp>
#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  template <Ring T>
  class Complex {
  public:
    // --- Construction ---

    Complex()
        : re_(identity_v<T, additive_tag>())
        , im_(identity_v<T, additive_tag>()) {}

    explicit Complex(T re)
        : re_(std::move(re))
        , im_(identity_v<T, additive_tag>()) {}

    Complex(T re, T im)
        : re_(std::move(re))
        , im_(std::move(im)) {}

    // --- Accessors ---

    [[nodiscard]] const T&
    real() const {
      return re_;
    }

    [[nodiscard]] const T&
    imag() const {
      return im_;
    }

    // --- Comparison ---

    friend bool
    operator==(const Complex& a, const Complex& b) {
      return a.re_ == b.re_ && a.im_ == b.im_;
    }

    // No operator<=> — complex numbers are not totally ordered.

    // --- Unary ---

    Complex
    operator-() const {
      return Complex(-re_, -im_);
    }

    Complex
    operator+() const {
      return *this;
    }

    // --- Addition ---

    friend Complex
    operator+(const Complex& a, const Complex& b) {
      return Complex(a.re_ + b.re_, a.im_ + b.im_);
    }

    // --- Subtraction ---

    friend Complex
    operator-(const Complex& a, const Complex& b) {
      return Complex(a.re_ - b.re_, a.im_ - b.im_);
    }

    // --- Multiplication ---
    // (a+bi)(c+di) = (ac-bd) + (ad+bc)i

    friend Complex
    operator*(const Complex& a, const Complex& b) {
      return Complex(
        a.re_ * b.re_ - a.im_ * b.im_, a.re_ * b.im_ + a.im_ * b.re_);
    }

    // --- Division (requires Field) ---
    // (a+bi)/(c+di) = ((ac+bd) + (bc-ad)i) / (c²+d²)

    friend Complex
    operator/(const Complex& a, const Complex& b)
      requires Field<T>
    {
      auto denom = b.re_ * b.re_ + b.im_ * b.im_;
      return Complex(
        (a.re_ * b.re_ + a.im_ * b.im_) / denom,
        (a.im_ * b.re_ - a.re_ * b.im_) / denom);
    }

    // --- Compound assignment ---

    Complex&
    operator+=(const Complex& other) {
      return *this = *this + other;
    }
    Complex&
    operator-=(const Complex& other) {
      return *this = *this - other;
    }
    Complex&
    operator*=(const Complex& other) {
      return *this = *this * other;
    }
    Complex&
    operator/=(const Complex& other)
      requires Field<T>
    {
      return *this = *this / other;
    }

    // --- Conjugate and norm ---

    friend Complex
    conjugate(const Complex& z) {
      return Complex(z.re_, -z.im_);
    }

    /// Norm (squared magnitude): |z|² = a² + b²
    /// Returns a value of the component type T.
    friend T
    norm(const Complex& z) {
      return z.re_ * z.re_ + z.im_ * z.im_;
    }

    // --- Stream output ---

    friend std::ostream&
    operator<<(std::ostream& os, const Complex& z) {
      os << "(" << z.re_ << "+" << z.im_ << "i)";
      return os;
    }

  private:
    T re_;
    T im_;
  };

  // --- identity_element specializations ---

  template <Ring T>
  struct identity_element<Complex<T>, additive_tag> {
    static Complex<T>
    make() {
      return Complex<T>();
    }
  };

  template <Ring T>
  struct identity_element<Complex<T>, multiplicative_tag> {
    static Complex<T>
    make() {
      return Complex<T>(identity_v<T, multiplicative_tag>());
    }
  };

} // namespace arithkit
