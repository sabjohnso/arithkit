#pragma once

/// @file polynomial.hpp
/// @brief Dense univariate polynomial over a CommutativeRing.
///
/// Polynomial<T> stores coefficients in ascending order:
/// coefficients[i] is the coefficient of x^i.
///
/// Models CommutativeRing always.  Models EuclideanDomain when T
/// models Field (polynomial long division becomes possible).

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <ostream>
#include <vector>

#include <arithkit/concepts/commutative_ring.hpp>
#include <arithkit/concepts/field.hpp>
#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  template <CommutativeRing T>
  class Polynomial {
  public:
    // --- Construction ---

    Polynomial() = default;

    explicit Polynomial(T scalar) {
      auto zero = identity_v<T, additive_tag>();
      if (!(scalar == zero)) { coeffs_.push_back(std::move(scalar)); }
    }

    explicit Polynomial(std::vector<T> coeffs)
        : coeffs_(std::move(coeffs)) {
      normalize();
    }

    Polynomial(std::initializer_list<T> coeffs)
        : coeffs_(coeffs) {
      normalize();
    }

    // --- Queries ---

    [[nodiscard]] bool
    is_zero() const {
      return coeffs_.empty();
    }

    [[nodiscard]] std::size_t
    degree() const {
      return coeffs_.empty() ? 0 : coeffs_.size() - 1;
    }

    // --- Coefficient access ---

    /// Returns coefficient of x^i.  Returns zero for i beyond degree.
    [[nodiscard]] T
    operator[](std::size_t i) const {
      if (i < coeffs_.size()) return coeffs_[i];
      return identity_v<T, additive_tag>();
    }

    // --- Evaluation (Horner's method) ---

    [[nodiscard]] T
    evaluate(const T& x) const {
      if (coeffs_.empty()) return identity_v<T, additive_tag>();
      T result = coeffs_.back();
      for (std::size_t i = coeffs_.size() - 1; i > 0; --i) {
        result = result * x + coeffs_[i - 1];
      }
      return result;
    }

    // --- Comparison ---

    friend bool
    operator==(const Polynomial& a, const Polynomial& b) {
      return a.coeffs_ == b.coeffs_;
    }

    // --- Unary ---

    Polynomial
    operator-() const {
      Polynomial result;
      result.coeffs_.reserve(coeffs_.size());
      for (const auto& c : coeffs_) {
        result.coeffs_.push_back(-c);
      }
      return result;
    }

    Polynomial
    operator+() const {
      return *this;
    }

    // --- Addition ---

    friend Polynomial
    operator+(const Polynomial& a, const Polynomial& b) {
      auto max_sz = std::max(a.coeffs_.size(), b.coeffs_.size());
      std::vector<T> result(max_sz, identity_v<T, additive_tag>());
      for (std::size_t i = 0; i < a.coeffs_.size(); ++i) {
        result[i] = result[i] + a.coeffs_[i];
      }
      for (std::size_t i = 0; i < b.coeffs_.size(); ++i) {
        result[i] = result[i] + b.coeffs_[i];
      }
      Polynomial p;
      p.coeffs_ = std::move(result);
      p.normalize();
      return p;
    }

    // --- Subtraction ---

    friend Polynomial
    operator-(const Polynomial& a, const Polynomial& b) {
      return a + (-b);
    }

    // --- Multiplication (Cauchy product) ---

    friend Polynomial
    operator*(const Polynomial& a, const Polynomial& b) {
      if (a.is_zero() || b.is_zero()) return Polynomial();
      auto zero = identity_v<T, additive_tag>();
      std::size_t result_sz = a.coeffs_.size() + b.coeffs_.size() - 1;
      std::vector<T> result(result_sz, zero);
      for (std::size_t i = 0; i < a.coeffs_.size(); ++i) {
        for (std::size_t j = 0; j < b.coeffs_.size(); ++j) {
          result[i + j] = result[i + j] + a.coeffs_[i] * b.coeffs_[j];
        }
      }
      Polynomial p;
      p.coeffs_ = std::move(result);
      p.normalize();
      return p;
    }

    // --- Division and remainder (requires Field<T>) ---

    friend Polynomial
    operator/(const Polynomial& a, const Polynomial& b)
      requires(Field<T>)
    {
      auto [q, r] = divmod(a, b);
      return q;
    }

    friend Polynomial
    operator%(const Polynomial& a, const Polynomial& b)
      requires(Field<T>)
    {
      auto [q, r] = divmod(a, b);
      return r;
    }

    // --- Mixed scalar operators ---

    friend Polynomial
    operator+(const Polynomial& p, const T& t) {
      return p + Polynomial(t);
    }
    friend Polynomial
    operator+(const T& t, const Polynomial& p) {
      return Polynomial(t) + p;
    }
    friend Polynomial
    operator-(const Polynomial& p, const T& t) {
      return p - Polynomial(t);
    }
    friend Polynomial
    operator-(const T& t, const Polynomial& p) {
      return Polynomial(t) - p;
    }
    friend Polynomial
    operator*(const Polynomial& p, const T& t) {
      return p * Polynomial(t);
    }
    friend Polynomial
    operator*(const T& t, const Polynomial& p) {
      return Polynomial(t) * p;
    }

    // --- Compound assignment ---

    Polynomial&
    operator+=(const Polynomial& other) {
      return *this = *this + other;
    }
    Polynomial&
    operator-=(const Polynomial& other) {
      return *this = *this - other;
    }
    Polynomial&
    operator*=(const Polynomial& other) {
      return *this = *this * other;
    }
    Polynomial&
    operator+=(const T& t) {
      return *this = *this + t;
    }
    Polynomial&
    operator-=(const T& t) {
      return *this = *this - t;
    }
    Polynomial&
    operator*=(const T& t) {
      return *this = *this * t;
    }

    // --- Stream output ---

    friend std::ostream&
    operator<<(std::ostream& os, const Polynomial& p) {
      if (p.is_zero()) {
        os << "0";
        return os;
      }
      bool first = true;
      for (std::size_t i = 0; i < p.coeffs_.size(); ++i) {
        if (!first) os << " + ";
        os << p.coeffs_[i];
        if (i == 1) os << "*x";
        if (i > 1) os << "*x^" << i;
        first = false;
      }
      return os;
    }

  private:
    std::vector<T> coeffs_;

    /// Remove trailing zero coefficients to maintain canonical form.
    void
    normalize() {
      auto zero = identity_v<T, additive_tag>();
      while (!coeffs_.empty() && coeffs_.back() == zero) {
        coeffs_.pop_back();
      }
    }

    /// Polynomial long division: returns (quotient, remainder).
    static std::pair<Polynomial, Polynomial>
    divmod(const Polynomial& a, const Polynomial& b)
      requires(Field<T>)
    {
      if (b.is_zero()) {
        // Division by zero — return zero (matching integer convention).
        return {Polynomial(), a};
      }
      if (a.is_zero() || a.degree() < b.degree()) { return {Polynomial(), a}; }

      auto one = identity_v<T, multiplicative_tag>();
      std::vector<T> rem(a.coeffs_);
      std::size_t q_sz = a.degree() - b.degree() + 1;
      std::vector<T> quotient(q_sz, identity_v<T, additive_tag>());

      T lead_inv = one / b.coeffs_.back();

      for (std::size_t i = q_sz; i > 0; --i) {
        std::size_t idx = i - 1;
        std::size_t rem_idx = idx + b.degree();
        T coeff = rem[rem_idx] * lead_inv;
        quotient[idx] = coeff;
        for (std::size_t j = 0; j < b.coeffs_.size(); ++j) {
          rem[idx + j] = rem[idx + j] - coeff * b.coeffs_[j];
        }
      }

      Polynomial qp;
      qp.coeffs_ = std::move(quotient);
      qp.normalize();

      Polynomial rp;
      rp.coeffs_ = std::move(rem);
      rp.normalize();

      return {qp, rp};
    }
  };

  // --- identity_element specializations ---

  template <CommutativeRing T>
  struct identity_element<Polynomial<T>, additive_tag> {
    static Polynomial<T>
    make() {
      return Polynomial<T>();
    }
  };

  template <CommutativeRing T>
  struct identity_element<Polynomial<T>, multiplicative_tag> {
    static Polynomial<T>
    make() {
      return Polynomial<T>(identity_v<T, multiplicative_tag>());
    }
  };

} // namespace arithkit
