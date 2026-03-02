#pragma once

/// @file quaternion.hpp
/// @brief Quaternion parameterized over any Field.
///
/// Represents w + xi + yj + zk where i²=j²=k²=ijk=−1.
/// Models DivisionRing (non-commutative multiplication).
/// NOT a Field — multiplication is not commutative.

#include <ostream>
#include <utility>

#include <arithkit/concepts/field.hpp>
#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  template <Field T>
  class Quaternion {
  public:
    // --- Construction ---

    Quaternion()
        : w_(identity_v<T, additive_tag>())
        , x_(identity_v<T, additive_tag>())
        , y_(identity_v<T, additive_tag>())
        , z_(identity_v<T, additive_tag>()) {}

    explicit Quaternion(T w)
        : w_(std::move(w))
        , x_(identity_v<T, additive_tag>())
        , y_(identity_v<T, additive_tag>())
        , z_(identity_v<T, additive_tag>()) {}

    Quaternion(T w, T x, T y, T z)
        : w_(std::move(w))
        , x_(std::move(x))
        , y_(std::move(y))
        , z_(std::move(z)) {}

    // --- Accessors ---

    [[nodiscard]] const T&
    w() const {
      return w_;
    }
    [[nodiscard]] const T&
    x() const {
      return x_;
    }
    [[nodiscard]] const T&
    y() const {
      return y_;
    }
    [[nodiscard]] const T&
    z() const {
      return z_;
    }

    // --- Comparison ---

    friend bool
    operator==(const Quaternion& a, const Quaternion& b) {
      return a.w_ == b.w_ && a.x_ == b.x_ && a.y_ == b.y_ && a.z_ == b.z_;
    }

    // No operator<=> — quaternions are not ordered.

    // --- Unary ---

    Quaternion
    operator-() const {
      return Quaternion(-w_, -x_, -y_, -z_);
    }

    Quaternion
    operator+() const {
      return *this;
    }

    // --- Addition ---

    friend Quaternion
    operator+(const Quaternion& a, const Quaternion& b) {
      return Quaternion(a.w_ + b.w_, a.x_ + b.x_, a.y_ + b.y_, a.z_ + b.z_);
    }

    // --- Subtraction ---

    friend Quaternion
    operator-(const Quaternion& a, const Quaternion& b) {
      return Quaternion(a.w_ - b.w_, a.x_ - b.x_, a.y_ - b.y_, a.z_ - b.z_);
    }

    // --- Multiplication (Hamilton product) ---
    // (a₁+b₁i+c₁j+d₁k)(a₂+b₂i+c₂j+d₂k) =
    //   (a₁a₂ - b₁b₂ - c₁c₂ - d₁d₂)
    // + (a₁b₂ + b₁a₂ + c₁d₂ - d₁c₂)i
    // + (a₁c₂ - b₁d₂ + c₁a₂ + d₁b₂)j
    // + (a₁d₂ + b₁c₂ - c₁b₂ + d₁a₂)k

    friend Quaternion
    operator*(const Quaternion& a, const Quaternion& b) {
      return Quaternion(
        a.w_ * b.w_ - a.x_ * b.x_ - a.y_ * b.y_ - a.z_ * b.z_,
        a.w_ * b.x_ + a.x_ * b.w_ + a.y_ * b.z_ - a.z_ * b.y_,
        a.w_ * b.y_ - a.x_ * b.z_ + a.y_ * b.w_ + a.z_ * b.x_,
        a.w_ * b.z_ + a.x_ * b.y_ - a.y_ * b.x_ + a.z_ * b.w_);
    }

    // --- Division ---
    // a / b = a * b⁻¹ where b⁻¹ = conj(b) / norm(b)

    friend Quaternion
    operator/(const Quaternion& a, const Quaternion& b) {
      auto n = norm(b);
      auto conj_b = conjugate(b);
      // b⁻¹ = conj(b) / norm(b)
      auto inv =
        Quaternion(conj_b.w_ / n, conj_b.x_ / n, conj_b.y_ / n, conj_b.z_ / n);
      return a * inv;
    }

    // --- Compound assignment ---

    Quaternion&
    operator+=(const Quaternion& other) {
      return *this = *this + other;
    }
    Quaternion&
    operator-=(const Quaternion& other) {
      return *this = *this - other;
    }
    Quaternion&
    operator*=(const Quaternion& other) {
      return *this = *this * other;
    }
    Quaternion&
    operator/=(const Quaternion& other) {
      return *this = *this / other;
    }

    // --- Conjugate and norm ---

    friend Quaternion
    conjugate(const Quaternion& q) {
      return Quaternion(q.w_, -q.x_, -q.y_, -q.z_);
    }

    /// Norm (squared magnitude): |q|² = w²+x²+y²+z²
    friend T
    norm(const Quaternion& q) {
      return q.w_ * q.w_ + q.x_ * q.x_ + q.y_ * q.y_ + q.z_ * q.z_;
    }

    // --- Stream output ---

    friend std::ostream&
    operator<<(std::ostream& os, const Quaternion& q) {
      os << "(" << q.w_ << "+" << q.x_ << "i+" << q.y_ << "j+" << q.z_ << "k)";
      return os;
    }

  private:
    T w_, x_, y_, z_;
  };

  // --- identity_element specializations ---

  template <Field T>
  struct identity_element<Quaternion<T>, additive_tag> {
    static Quaternion<T>
    make() {
      return Quaternion<T>();
    }
  };

  template <Field T>
  struct identity_element<Quaternion<T>, multiplicative_tag> {
    static Quaternion<T>
    make() {
      return Quaternion<T>(identity_v<T, multiplicative_tag>());
    }
  };

} // namespace arithkit
