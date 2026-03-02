#pragma once

/// @file additive_monoid_laws.hpp
/// @brief Property tests for AdditiveMonoid (identity element for +).

#include <arithkit/concepts/additive_monoid.hpp>
#include <arithkit/traits/identity_element.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "additive_semigroup_laws.hpp"

namespace arithkit::laws {

  template <AdditiveMonoid T>
  void
  check_additive_monoid() {
    check_additive_semigroup<T>();

    rc::prop("additive identity: a + 0 == a && 0 + a == a", [](T a) {
      auto zero = identity_v<T, additive_tag>();
      RC_ASSERT(a + zero == a);
      RC_ASSERT(zero + a == a);
    });
  }

} // namespace arithkit::laws
