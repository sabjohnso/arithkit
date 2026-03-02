#pragma once

/// @file ring_laws.hpp
/// @brief Property tests for Ring (distributivity).

#include <arithkit/concepts/ring.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "additive_abelian_group_laws.hpp"
#include "multiplicative_monoid_laws.hpp"

namespace arithkit::laws {

  template <Ring T>
  void
  check_ring() {
    check_additive_abelian_group<T>();
    check_multiplicative_monoid<T>();

    rc::prop("left distributivity: a*(b+c) == a*b + a*c", [](T a, T b, T c) {
      RC_ASSERT(a * (b + c) == a * b + a * c);
    });

    rc::prop("right distributivity: (a+b)*c == a*c + b*c", [](T a, T b, T c) {
      RC_ASSERT((a + b) * c == a * c + b * c);
    });
  }

} // namespace arithkit::laws
