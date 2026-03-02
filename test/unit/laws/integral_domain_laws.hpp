#pragma once

/// @file integral_domain_laws.hpp
/// @brief Property tests for IntegralDomain (no zero divisors).

#include <arithkit/concepts/integral_domain.hpp>
#include <arithkit/traits/identity_element.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "commutative_ring_laws.hpp"

namespace arithkit::laws {

  template <IntegralDomain T>
  void
  check_integral_domain() {
    check_commutative_ring<T>();

    rc::prop(
      "no zero divisors: a*b == 0 implies a == 0 || b == 0", [](T a, T b) {
        auto zero = identity_v<T, additive_tag>();
        if (a * b == zero) { RC_ASSERT(a == zero || b == zero); }
      });
  }

} // namespace arithkit::laws
