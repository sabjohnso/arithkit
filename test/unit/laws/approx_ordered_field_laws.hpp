#pragma once

/// @file approx_ordered_field_laws.hpp
/// @brief Approximate property tests for floating-point ordered fields.
///
/// Floating-point types don't satisfy exact algebraic laws due to
/// rounding.  These suites use approximate equality (relative +
/// absolute tolerance) to verify laws hold within machine precision.

#include <cmath>

#include <arithkit/concepts/ordered_field.hpp>
#include <arithkit/traits/identity_element.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "approx_equal.hpp"

namespace arithkit::laws {

  namespace detail {
    using std::abs;
    using std::isfinite;

    template <typename T>
    bool
    is_finite(const T& v) {
      return isfinite(v);
    }

    template <typename T>
    T
    abs_val(const T& v) {
      return abs(v);
    }
  } // namespace detail

  template <OrderedField T>
  void
  check_approx_ordered_field() {
    auto is_fin = [](const T& v) { return detail::is_finite(v); };

    // --- Additive semigroup: associativity ---
    rc::prop("approx additive associativity", [=](T a, T b, T c) {
      RC_PRE(is_fin(a) && is_fin(b) && is_fin(c));
      auto lhs = (a + b) + c;
      auto rhs = a + (b + c);
      RC_PRE(is_fin(lhs) && is_fin(rhs));
      RC_ASSERT(approx_equal(lhs, rhs));
    });

    // --- Additive monoid: identity ---
    rc::prop("approx additive identity", [=](T a) {
      RC_PRE(is_fin(a));
      auto zero = identity_v<T, additive_tag>();
      RC_ASSERT(a + zero == a);
      RC_ASSERT(zero + a == a);
    });

    // --- Additive group: inverse ---
    rc::prop("approx additive inverse", [=](T a) {
      RC_PRE(is_fin(a));
      auto zero = identity_v<T, additive_tag>();
      RC_ASSERT(a + (-a) == zero);
      RC_ASSERT((-a) + a == zero);
    });

    // --- Additive abelian group: commutativity ---
    rc::prop("approx additive commutativity", [=](T a, T b) {
      RC_PRE(is_fin(a) && is_fin(b));
      auto sum = a + b;
      RC_PRE(is_fin(sum));
      RC_ASSERT(a + b == b + a);
    });

    // --- Multiplicative semigroup: associativity ---
    rc::prop("approx multiplicative associativity", [=](T a, T b, T c) {
      RC_PRE(is_fin(a) && is_fin(b) && is_fin(c));
      auto lhs = (a * b) * c;
      auto rhs = a * (b * c);
      RC_PRE(is_fin(lhs) && is_fin(rhs));
      RC_ASSERT(approx_equal(lhs, rhs));
    });

    // --- Multiplicative monoid: identity ---
    rc::prop("approx multiplicative identity", [=](T a) {
      RC_PRE(is_fin(a));
      auto one = identity_v<T, multiplicative_tag>();
      RC_ASSERT(a * one == a);
      RC_ASSERT(one * a == a);
    });

    // --- Ring: distributivity ---
    rc::prop("approx left distributivity", [=](T a, T b, T c) {
      RC_PRE(is_fin(a) && is_fin(b) && is_fin(c));
      auto lhs = a * (b + c);
      auto rhs = a * b + a * c;
      RC_PRE(is_fin(lhs) && is_fin(rhs));
      RC_ASSERT(approx_equal(lhs, rhs));
    });

    rc::prop("approx right distributivity", [=](T a, T b, T c) {
      RC_PRE(is_fin(a) && is_fin(b) && is_fin(c));
      auto lhs = (a + b) * c;
      auto rhs = a * c + b * c;
      RC_PRE(is_fin(lhs) && is_fin(rhs));
      RC_ASSERT(approx_equal(lhs, rhs));
    });

    // --- Commutative ring: multiplicative commutativity ---
    rc::prop("approx multiplicative commutativity", [=](T a, T b) {
      RC_PRE(is_fin(a) && is_fin(b));
      auto prod = a * b;
      RC_PRE(is_fin(prod));
      RC_ASSERT(a * b == b * a);
    });

    // --- Field: multiplicative inverse ---
    rc::prop("approx multiplicative inverse", [=](T a) {
      RC_PRE(is_fin(a));
      auto zero = identity_v<T, additive_tag>();
      RC_PRE(!(a == zero));
      auto one = identity_v<T, multiplicative_tag>();
      auto result = a * (one / a);
      RC_PRE(is_fin(result));
      RC_ASSERT(approx_equal(result, one));
    });

    // --- Ordered field: total order ---
    rc::prop("total order: a < b || a == b || a > b", [=](T a, T b) {
      RC_PRE(is_fin(a) && is_fin(b));
      RC_ASSERT(a < b || a == b || a > b);
    });

    // --- Ordered field: order-addition compatibility ---
    rc::prop("order-addition compatibility", [=](T a, T b, T c) {
      RC_PRE(is_fin(a) && is_fin(b) && is_fin(c));
      RC_PRE(is_fin(a + c) && is_fin(b + c));
      if (a < b) { RC_ASSERT(a + c <= b + c); }
    });

    // --- Ordered field: order-multiplication compatibility ---
    rc::prop("order-mult compatibility: 0<a && 0<b => 0<a*b", [=](T a, T b) {
      RC_PRE(is_fin(a) && is_fin(b));
      auto zero = identity_v<T, additive_tag>();
      if (a > zero && b > zero) {
        auto prod = a * b;
        RC_PRE(is_fin(prod));
        RC_ASSERT(prod > zero);
      }
    });
  }

} // namespace arithkit::laws
