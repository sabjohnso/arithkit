#pragma once

/// @file ordered_field_laws.hpp
/// @brief Property tests for OrderedField (total order + field).

#include <arithkit/concepts/ordered_field.hpp>
#include <arithkit/traits/identity_element.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "field_laws.hpp"

namespace arithkit::laws {

  template <OrderedField T>
  void
  check_ordered_field() {
    check_field<T>();

    rc::prop("total order: a < b || a == b || a > b", [](T a, T b) {
      RC_ASSERT(a < b || a == b || a > b);
    });

    rc::prop(
      "order-addition compatibility: a < b implies a+c < b+c",
      [](T a, T b, T c) {
        if (a < b) { RC_ASSERT(a + c < b + c); }
      });

    rc::prop(
      "order-multiplication compatibility: 0<a && 0<b implies 0<a*b",
      [](T a, T b) {
        auto zero = identity_v<T, additive_tag>();
        if (a > zero && b > zero) { RC_ASSERT(a * b > zero); }
      });
  }

} // namespace arithkit::laws
