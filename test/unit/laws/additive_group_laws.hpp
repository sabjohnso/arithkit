#pragma once

/// @file additive_group_laws.hpp
/// @brief Property tests for AdditiveGroup (additive inverse).

#include <arithkit/concepts/additive_group.hpp>
#include <arithkit/traits/identity_element.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "additive_monoid_laws.hpp"

namespace arithkit::laws {

  template <AdditiveGroup T>
  void
  check_additive_group() {
    check_additive_monoid<T>();

    rc::prop("additive inverse: a + (-a) == 0 && (-a) + a == 0", [](T a) {
      auto zero = identity_v<T, additive_tag>();
      RC_ASSERT(a + (-a) == zero);
      RC_ASSERT((-a) + a == zero);
    });
  }

} // namespace arithkit::laws
