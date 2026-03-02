#pragma once

/// @file multiplicative_semigroup_laws.hpp
/// @brief Property tests for MultiplicativeSemigroup (associativity of *).

#include <arithkit/concepts/multiplicative_semigroup.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

namespace arithkit::laws {

  template <MultiplicativeSemigroup T>
  void
  check_multiplicative_semigroup() {
    rc::prop(
      "multiplicative associativity: (a*b)*c == a*(b*c)",
      [](T a, T b, T c) { RC_ASSERT((a * b) * c == a * (b * c)); });
  }

} // namespace arithkit::laws
