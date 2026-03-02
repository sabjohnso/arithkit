#pragma once

/// @file additive_abelian_group_laws.hpp
/// @brief Property tests for AdditiveAbelianGroup (commutativity of +).

#include <arithkit/concepts/additive_abelian_group.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "additive_group_laws.hpp"

namespace arithkit::laws {

  template <AdditiveAbelianGroup T>
  void
  check_additive_abelian_group() {
    check_additive_group<T>();

    rc::prop("additive commutativity: a + b == b + a", [](T a, T b) {
      RC_ASSERT(a + b == b + a);
    });
  }

} // namespace arithkit::laws
