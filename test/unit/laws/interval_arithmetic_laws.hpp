#pragma once

/// @file interval_arithmetic_laws.hpp
/// @brief Interval arithmetic laws: containment and subdistributivity.
///
/// Interval arithmetic is NOT a ring — distributivity only holds as subset
/// inclusion (subdistributivity). These laws verify that arithmetic operations
/// on intervals contain the corresponding pointwise results.

#include <arithkit/concepts/ordered_field.hpp>
#include <arithkit/interval.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

namespace arithkit::laws {

  template <OrderedField T>
  void
  check_interval_arithmetic() {
    using Iv = Interval<T>;

    rc::prop(
      "addition containment: x in X, y in Y => x+y in X+Y", [](Iv X, Iv Y) {
        auto x = X.midpoint();
        auto y = Y.midpoint();
        auto result = X + Y;
        RC_ASSERT(result.contains(x + y));
      });

    rc::prop(
      "subtraction containment: x in X, y in Y => x-y in X-Y", [](Iv X, Iv Y) {
        auto x = X.midpoint();
        auto y = Y.midpoint();
        auto result = X - Y;
        RC_ASSERT(result.contains(x - y));
      });

    rc::prop(
      "multiplication containment: x in X, y in Y => x*y in X*Y",
      [](Iv X, Iv Y) {
        auto x = X.midpoint();
        auto y = Y.midpoint();
        auto result = X * Y;
        RC_ASSERT(result.contains(x * y));
      });

    rc::prop(
      "subdistributivity: X*(Y+Z) subset X*Y + X*Z", [](Iv X, Iv Y, Iv Z) {
        auto lhs = X * (Y + Z);
        auto rhs = X * Y + X * Z;
        RC_ASSERT(lhs.is_subset_of(rhs));
      });
  }

} // namespace arithkit::laws
