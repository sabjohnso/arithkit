#pragma once

/// @file sparsity.hpp
/// @brief Scalar with a dependency index set for sparsity pattern extraction.
///
/// Sparsity<T> wraps a scalar value together with a set of variable indices.
/// All four arithmetic operations propagate the index set via union:
///
///   (v1,S1) op (v2,S2) = (v1 op v2, S1 ∪ S2)
///
/// This type is NOT a ring: the additive inverse law fails because
///   (v,S) + (-v,S) = (0,S) ≠ (0,∅) when S is nonempty.
///
/// Intended use: tag each unknown variable with a unique index, run a
/// computation symbolically, then read off the dependency pattern from
/// the result's index set.

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <ostream>
#include <set>

namespace arithkit {

  template <typename T>
  class Sparsity {
  public:
    // --- Construction ---

    Sparsity()
        : value_{}
        , indices_{} {}

    explicit Sparsity(T val)
        : value_{std::move(val)}
        , indices_{} {}

    Sparsity(T val, std::initializer_list<std::size_t> idx)
        : value_{std::move(val)}
        , indices_{idx} {}

    Sparsity(T val, std::set<std::size_t> idx)
        : value_{std::move(val)}
        , indices_{std::move(idx)} {}

    // --- Accessors ---

    [[nodiscard]] const T&
    value() const {
      return value_;
    }

    [[nodiscard]] const std::set<std::size_t>&
    indices() const {
      return indices_;
    }

    // --- Unary negation ---

    Sparsity
    operator-() const {
      return Sparsity(-value_, indices_);
    }

    // --- Binary arithmetic: value op, indices union ---

    friend Sparsity
    operator+(const Sparsity& a, const Sparsity& b) {
      return Sparsity(a.value_ + b.value_, union_of(a.indices_, b.indices_));
    }

    friend Sparsity
    operator-(const Sparsity& a, const Sparsity& b) {
      return Sparsity(a.value_ - b.value_, union_of(a.indices_, b.indices_));
    }

    friend Sparsity
    operator*(const Sparsity& a, const Sparsity& b) {
      return Sparsity(a.value_ * b.value_, union_of(a.indices_, b.indices_));
    }

    friend Sparsity
    operator/(const Sparsity& a, const Sparsity& b)
      requires requires(T x, T y) { x / y; }
    {
      return Sparsity(a.value_ / b.value_, union_of(a.indices_, b.indices_));
    }

    // --- Mixed scalar/sparsity operators (scalar t ≡ Sparsity(t)) ---

    friend Sparsity
    operator+(const Sparsity& z, const T& t) {
      return z + Sparsity(t);
    }
    friend Sparsity
    operator+(const T& t, const Sparsity& z) {
      return Sparsity(t) + z;
    }
    friend Sparsity
    operator-(const Sparsity& z, const T& t) {
      return z - Sparsity(t);
    }
    friend Sparsity
    operator-(const T& t, const Sparsity& z) {
      return Sparsity(t) - z;
    }
    friend Sparsity
    operator*(const Sparsity& z, const T& t) {
      return z * Sparsity(t);
    }
    friend Sparsity
    operator*(const T& t, const Sparsity& z) {
      return Sparsity(t) * z;
    }
    friend Sparsity
    operator/(const Sparsity& z, const T& t)
      requires requires(T x, T y) { x / y; }
    {
      return z / Sparsity(t);
    }
    friend Sparsity
    operator/(const T& t, const Sparsity& z)
      requires requires(T x, T y) { x / y; }
    {
      return Sparsity(t) / z;
    }

    // --- Compound assignment ---

    Sparsity&
    operator+=(const Sparsity& other) {
      return *this = *this + other;
    }

    Sparsity&
    operator-=(const Sparsity& other) {
      return *this = *this - other;
    }

    Sparsity&
    operator*=(const Sparsity& other) {
      return *this = *this * other;
    }

    Sparsity&
    operator/=(const Sparsity& other) {
      return *this = *this / other;
    }

    Sparsity&
    operator+=(const T& t) {
      return *this = *this + t;
    }

    Sparsity&
    operator-=(const T& t) {
      return *this = *this - t;
    }

    Sparsity&
    operator*=(const T& t) {
      return *this = *this * t;
    }

    Sparsity&
    operator/=(const T& t)
      requires requires(T x, T y) { x / y; }
    {
      return *this = *this / t;
    }

    // --- Equality ---

    friend bool
    operator==(const Sparsity& a, const Sparsity& b) {
      return a.value_ == b.value_ && a.indices_ == b.indices_;
    }

    // --- Stream output ---

    friend std::ostream&
    operator<<(std::ostream& os, const Sparsity& s) {
      os << "(" << s.value_ << ", {";
      bool first = true;
      for (auto idx : s.indices_) {
        if (!first) os << ", ";
        os << idx;
        first = false;
      }
      os << "})";
      return os;
    }

  private:
    T value_;
    std::set<std::size_t> indices_;

    [[nodiscard]] static std::set<std::size_t>
    union_of(const std::set<std::size_t>& a, const std::set<std::size_t>& b) {
      std::set<std::size_t> result;
      std::set_union(
        a.begin(),
        a.end(),
        b.begin(),
        b.end(),
        std::inserter(result, result.begin()));
      return result;
    }
  };

} // namespace arithkit
