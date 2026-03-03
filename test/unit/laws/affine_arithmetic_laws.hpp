#pragma once

/// @file affine_arithmetic_laws.hpp
/// @brief Affine arithmetic laws: containment and tighter-than-interval bounds.
///
/// Affine arithmetic tracks linear correlations via noise symbols, producing
/// tighter enclosures than naive interval arithmetic. These laws verify
/// containment of pointwise results and the tighter-bounds invariant.

#include <arithkit/affine.hpp>
#include <arithkit/concepts/ordered_field.hpp>
#include <arithkit/interval.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

namespace arithkit::laws {

  template <OrderedField T>
  void
  check_affine_arithmetic() {
    using A = Affine<T>;

    rc::prop(
      "addition containment: center sum in (X+Y).to_interval()", [](A X, A Y) {
        auto x = X.center();
        auto y = Y.center();
        auto result = X + Y;
        RC_ASSERT(result.to_interval().contains(x + y));
      });

    rc::prop(
      "subtraction containment: center diff in (X-Y).to_interval()",
      [](A X, A Y) {
        auto x = X.center();
        auto y = Y.center();
        auto result = X - Y;
        RC_ASSERT(result.to_interval().contains(x - y));
      });

    rc::prop(
      "multiplication containment: center product in (X*Y).to_interval()",
      [](A X, A Y) {
        auto x = X.center();
        auto y = Y.center();
        auto result = X * Y;
        RC_ASSERT(result.to_interval().contains(x * y));
      });

    rc::prop(
      "tighter bounds: (X+Y).to_interval() subset "
      "X.to_interval() + Y.to_interval()",
      [](A X, A Y) {
        auto xiv = X.to_interval();
        auto yiv = Y.to_interval();
        auto affine_result = (X + Y).to_interval();
        auto interval_result = xiv + yiv;
        RC_ASSERT(affine_result.is_subset_of(interval_result));
      });
  }

} // namespace arithkit::laws
