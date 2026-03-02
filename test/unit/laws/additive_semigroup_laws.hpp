#pragma once

/// @file additive_semigroup_laws.hpp
/// @brief Property tests for AdditiveSemigroup (associativity of +).

#include <arithkit/concepts/additive_semigroup.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

namespace arithkit::laws {

  template <AdditiveSemigroup T>
  void
  check_additive_semigroup() {
    rc::prop("additive associativity: (a+b)+c == a+(b+c)", [](T a, T b, T c) {
      RC_ASSERT((a + b) + c == a + (b + c));
    });
  }

} // namespace arithkit::laws
