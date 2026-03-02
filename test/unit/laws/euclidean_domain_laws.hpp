#pragma once

/// @file euclidean_domain_laws.hpp
/// @brief Property tests for EuclideanDomain (Euclidean division).

#include <arithkit/concepts/euclidean_domain.hpp>
#include <arithkit/traits/identity_element.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "integral_domain_laws.hpp"

namespace arithkit::laws {

  template <EuclideanDomain T>
  void
  check_euclidean_domain() {
    check_integral_domain<T>();

    rc::prop("division algorithm: a == (a/b)*b + a%b for b != 0", [](T a, T b) {
      auto zero = identity_v<T, additive_tag>();
      RC_PRE(b != zero);
      RC_ASSERT(a == (a / b) * b + a % b);
    });
  }

} // namespace arithkit::laws
