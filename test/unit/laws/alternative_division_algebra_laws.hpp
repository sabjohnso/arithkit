#pragma once

/// @file alternative_division_algebra_laws.hpp
/// @brief Property tests for AlternativeDivisionAlgebra (alternativity,
///        flexibility, multiplicative inverse).

#include <arithkit/concepts/alternative_division_algebra.hpp>
#include <arithkit/traits/identity_element.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "non_associative_ring_laws.hpp"

namespace arithkit::laws {

  template <AlternativeDivisionAlgebra T>
  void
  check_alternative_division_algebra() {
    check_non_associative_ring<T>();

    rc::prop("left alternativity: a*(a*b) == (a*a)*b", [](T a, T b) {
      RC_ASSERT(a * (a * b) == (a * a) * b);
    });

    rc::prop("right alternativity: (a*b)*b == a*(b*b)", [](T a, T b) {
      RC_ASSERT((a * b) * b == a * (b * b));
    });

    rc::prop("flexibility: a*(b*a) == (a*b)*a", [](T a, T b) {
      RC_ASSERT(a * (b * a) == (a * b) * a);
    });

    rc::prop("moufang 1: a*(b*(a*c)) == (a*(b*a))*c", [](T a, T b, T c) {
      RC_ASSERT(a * (b * (a * c)) == (a * (b * a)) * c);
    });

    rc::prop("moufang 2: ((a*b)*c)*b == a*(b*(c*b))", [](T a, T b, T c) {
      RC_ASSERT(((a * b) * c) * b == a * (b * (c * b)));
    });

    rc::prop("moufang 3: (a*b)*(c*a) == a*((b*c)*a)", [](T a, T b, T c) {
      RC_ASSERT((a * b) * (c * a) == a * ((b * c) * a));
    });

    rc::prop("multiplicative inverse: a * (1/a) == 1 for a != 0", [](T a) {
      auto zero = identity_v<T, additive_tag>();
      auto one = identity_v<T, multiplicative_tag>();
      RC_PRE(a != zero);
      RC_ASSERT(a * (one / a) == one);
      RC_ASSERT((one / a) * a == one);
    });
  }

} // namespace arithkit::laws
