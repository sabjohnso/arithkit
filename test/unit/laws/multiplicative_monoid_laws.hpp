#pragma once

/// @file multiplicative_monoid_laws.hpp
/// @brief Property tests for MultiplicativeMonoid (multiplicative identity).

#include <arithkit/concepts/multiplicative_monoid.hpp>
#include <arithkit/traits/identity_element.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "multiplicative_semigroup_laws.hpp"

namespace arithkit::laws {

  template <MultiplicativeMonoid T>
  void
  check_multiplicative_monoid() {
    check_multiplicative_semigroup<T>();

    rc::prop("multiplicative identity: a * 1 == a && 1 * a == a", [](T a) {
      auto one = identity_v<T, multiplicative_tag>();
      RC_ASSERT(a * one == a);
      RC_ASSERT(one * a == a);
    });
  }

} // namespace arithkit::laws
