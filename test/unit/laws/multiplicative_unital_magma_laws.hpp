#pragma once

/// @file multiplicative_unital_magma_laws.hpp
/// @brief Property tests for MultiplicativeUnitalMagma (identity only,
///        no associativity).

#include <arithkit/concepts/multiplicative_unital_magma.hpp>
#include <arithkit/traits/identity_element.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

namespace arithkit::laws {

  template <MultiplicativeUnitalMagma T>
  void
  check_multiplicative_unital_magma() {
    rc::prop("multiplicative identity: a * 1 == a && 1 * a == a", [](T a) {
      auto one = identity_v<T, multiplicative_tag>();
      RC_ASSERT(a * one == a);
      RC_ASSERT(one * a == a);
    });
  }

} // namespace arithkit::laws
