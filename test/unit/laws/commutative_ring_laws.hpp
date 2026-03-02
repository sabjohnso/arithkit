#pragma once

/// @file commutative_ring_laws.hpp
/// @brief Property tests for CommutativeRing (multiplicative commutativity).

#include <arithkit/concepts/commutative_ring.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "ring_laws.hpp"

namespace arithkit::laws {

  template <CommutativeRing T>
  void
  check_commutative_ring() {
    check_ring<T>();

    rc::prop("multiplicative commutativity: a * b == b * a", [](T a, T b) {
      RC_ASSERT(a * b == b * a);
    });
  }

} // namespace arithkit::laws
