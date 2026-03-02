#pragma once

/// @file division_ring_laws.hpp
/// @brief Property tests for DivisionRing (multiplicative inverse).

#include <arithkit/concepts/division_ring.hpp>
#include <arithkit/traits/identity_element.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "ring_laws.hpp"

namespace arithkit::laws {

  template <DivisionRing T>
  void
  check_division_ring() {
    check_ring<T>();

    rc::prop("multiplicative inverse: a * (1/a) == 1 for a != 0", [](T a) {
      auto zero = identity_v<T, additive_tag>();
      auto one = identity_v<T, multiplicative_tag>();
      RC_PRE(a != zero);
      RC_ASSERT(a * (one / a) == one);
      RC_ASSERT((one / a) * a == one);
    });
  }

} // namespace arithkit::laws
