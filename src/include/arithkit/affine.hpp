#pragma once

/// @file affine.hpp
/// @brief Affine arithmetic over any OrderedField.
///
/// Represents x₀ + Σ xᵢεᵢ where εᵢ ∈ [-1,1] are noise symbols.
/// Tracks correlations between quantities that share noise symbols,
/// giving tighter bounds than plain interval arithmetic when inputs
/// have common sources of uncertainty.
///
/// Models IntervalArithmetic (NOT a ring — same as Interval).

#include <algorithm>
#include <cmath>
#include <ostream>
#include <utility>
#include <vector>

#include <arithkit/concepts/ordered_field.hpp>
#include <arithkit/interval.hpp>
#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  template <OrderedField T>
  class Affine {
  public:
    // --- Construction ---

    Affine()
        : center_(identity_v<T, additive_tag>()) {}

    explicit Affine(T center)
        : center_(std::move(center)) {}

    Affine(T center, std::vector<T> noise)
        : center_(std::move(center))
        , noise_(std::move(noise)) {}

    // --- Accessors ---

    [[nodiscard]] const T&
    center() const {
      return center_;
    }

    [[nodiscard]] const std::vector<T>&
    noise_terms() const {
      return noise_;
    }

    /// Convert to interval by bounding: x₀ ± Σ|xᵢ|
    [[nodiscard]] Interval<T>
    to_interval() const {
      auto radius = compute_radius();
      return Interval<T>(center_ - radius, center_ + radius);
    }

    // --- Comparison ---

    friend bool
    operator==(const Affine& a, const Affine& b) {
      return a.center_ == b.center_ && a.noise_ == b.noise_;
    }

    // --- Unary ---

    Affine
    operator-() const {
      std::vector<T> neg_noise;
      neg_noise.reserve(noise_.size());
      for (const auto& n : noise_) {
        neg_noise.push_back(-n);
      }
      return Affine(-center_, std::move(neg_noise));
    }

    Affine
    operator+() const {
      return *this;
    }

    // --- Addition ---
    // (x₀ + Σ xᵢεᵢ) + (y₀ + Σ yᵢεᵢ) = (x₀+y₀) + Σ (xᵢ+yᵢ)εᵢ

    friend Affine
    operator+(const Affine& a, const Affine& b) {
      auto noise = add_noise(a.noise_, b.noise_);
      return Affine(a.center_ + b.center_, std::move(noise));
    }

    // --- Subtraction ---

    friend Affine
    operator-(const Affine& a, const Affine& b) {
      auto noise = sub_noise(a.noise_, b.noise_);
      return Affine(a.center_ - b.center_, std::move(noise));
    }

    // --- Multiplication ---
    // (x₀ + Σ xᵢεᵢ)(y₀ + Σ yᵢεᵢ) ≈
    //   x₀·y₀ + Σ (x₀·yᵢ + y₀·xᵢ)εᵢ + new noise for nonlinear terms

    friend Affine
    operator*(const Affine& a, const Affine& b) {
      auto zero = identity_v<T, additive_tag>();

      // Linear terms
      auto sz = std::max(a.noise_.size(), b.noise_.size());
      std::vector<T> noise(sz, zero);
      for (std::size_t i = 0; i < sz; ++i) {
        auto ai = i < a.noise_.size() ? a.noise_[i] : zero;
        auto bi = i < b.noise_.size() ? b.noise_[i] : zero;
        noise[i] = a.center_ * bi + b.center_ * ai;
      }

      // Nonlinear remainder: bound Σᵢ Σⱼ xᵢyⱼ εᵢεⱼ
      // Conservative bound: radius_a * radius_b
      auto nonlinear = a.compute_radius() * b.compute_radius();
      if (!(nonlinear == zero)) { noise.push_back(nonlinear); }

      return Affine(a.center_ * b.center_, std::move(noise));
    }

    // --- Division ---
    // Approximated as a * (1/b) using affine reciprocal linearization.
    // For simplicity, uses interval division fallback for correctness.

    friend Affine
    operator/(const Affine& a, const Affine& b) {
      auto zero = identity_v<T, additive_tag>();
      auto one = identity_v<T, multiplicative_tag>();

      // If b has no noise, it's a scalar division
      if (b.noise_.empty() || b.compute_radius() == zero) {
        std::vector<T> noise;
        noise.reserve(a.noise_.size());
        for (const auto& n : a.noise_) {
          noise.push_back(n / b.center_);
        }
        return Affine(a.center_ / b.center_, std::move(noise));
      }

      // General case: linearize 1/b around b.center_
      // 1/b ≈ 1/b₀ - (b-b₀)/b₀²
      // = (2/b₀) - b/b₀²
      auto b0 = b.center_;
      auto b0_sq = b0 * b0;
      auto inv_center = one / b0;

      // Noise terms of reciprocal: -bᵢ/b₀²
      std::vector<T> inv_noise;
      inv_noise.reserve(b.noise_.size());
      for (const auto& n : b.noise_) {
        inv_noise.push_back(-(n / b0_sq));
      }

      // Nonlinear error bound for reciprocal
      auto b_radius = b.compute_radius();
      auto b_lo = b0 - b_radius;
      auto b_hi = b0 + b_radius;

      // Ensure divisor interval doesn't contain zero
      if (b_lo <= zero && zero <= b_hi) {
        // Fallback: use interval arithmetic
        auto a_iv = a.to_interval();
        auto b_iv = b.to_interval();
        auto result_iv = a_iv / b_iv;
        return from_interval(result_iv);
      }

      auto nonlinear = b_radius * b_radius / (abs_val(b_lo) * abs_val(b_hi));
      if (!(nonlinear == zero)) { inv_noise.push_back(nonlinear); }

      auto inv_b = Affine(inv_center, std::move(inv_noise));
      return a * inv_b;
    }

    // --- Compound assignment ---

    Affine&
    operator+=(const Affine& other) {
      return *this = *this + other;
    }
    Affine&
    operator-=(const Affine& other) {
      return *this = *this - other;
    }
    Affine&
    operator*=(const Affine& other) {
      return *this = *this * other;
    }
    Affine&
    operator/=(const Affine& other) {
      return *this = *this / other;
    }

    // --- Stream output ---

    friend std::ostream&
    operator<<(std::ostream& os, const Affine& a) {
      os << a.center_;
      for (std::size_t i = 0; i < a.noise_.size(); ++i) {
        os << " + " << a.noise_[i] << "ε" << i;
      }
      return os;
    }

  private:
    T center_;
    std::vector<T> noise_;

    /// Sum of absolute values of noise terms.
    [[nodiscard]] T
    compute_radius() const {
      auto zero = identity_v<T, additive_tag>();
      auto result = zero;
      for (const auto& n : noise_) {
        result = result + abs_val(n);
      }
      return result;
    }

    static T
    abs_val(const T& x) {
      auto zero = identity_v<T, additive_tag>();
      return x < zero ? -x : x;
    }

    /// Component-wise addition of noise vectors.
    static std::vector<T>
    add_noise(const std::vector<T>& a, const std::vector<T>& b) {
      auto zero = identity_v<T, additive_tag>();
      auto sz = std::max(a.size(), b.size());
      std::vector<T> result(sz, zero);
      for (std::size_t i = 0; i < sz; ++i) {
        auto ai = i < a.size() ? a[i] : zero;
        auto bi = i < b.size() ? b[i] : zero;
        result[i] = ai + bi;
      }
      return result;
    }

    /// Component-wise subtraction of noise vectors.
    static std::vector<T>
    sub_noise(const std::vector<T>& a, const std::vector<T>& b) {
      auto zero = identity_v<T, additive_tag>();
      auto sz = std::max(a.size(), b.size());
      std::vector<T> result(sz, zero);
      for (std::size_t i = 0; i < sz; ++i) {
        auto ai = i < a.size() ? a[i] : zero;
        auto bi = i < b.size() ? b[i] : zero;
        result[i] = ai - bi;
      }
      return result;
    }

    /// Create an Affine from an Interval (single noise symbol).
    static Affine
    from_interval(const Interval<T>& iv) {
      auto two = identity_v<T, multiplicative_tag>() +
                 identity_v<T, multiplicative_tag>();
      auto center = (iv.lo() + iv.hi()) / two;
      auto radius = (iv.hi() - iv.lo()) / two;
      auto zero = identity_v<T, additive_tag>();
      if (radius == zero) return Affine(center);
      return Affine(center, {radius});
    }
  };

  // --- identity_element specializations ---

  template <OrderedField T>
  struct identity_element<Affine<T>, additive_tag> {
    static Affine<T>
    make() {
      return Affine<T>();
    }
  };

  template <OrderedField T>
  struct identity_element<Affine<T>, multiplicative_tag> {
    static Affine<T>
    make() {
      return Affine<T>(identity_v<T, multiplicative_tag>());
    }
  };

} // namespace arithkit
