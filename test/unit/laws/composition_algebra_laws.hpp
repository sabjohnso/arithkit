#pragma once

/// @file composition_algebra_laws.hpp
/// @brief Composition algebra laws shared by Quaternion, Octonion, and Complex.
///
/// These are the structural laws of a composition algebra beyond its generic
/// algebraic axioms: conjugation as an anti-involution and the norm as a
/// multiplicative quadratic form satisfying z·conj(z) = norm(z).

#include <arithkit/traits/identity_element.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

namespace arithkit::laws {

  /// Check composition algebra laws for algebra E over scalar T.
  /// Requires: conjugate(E) -> E, norm(E) -> T, E(T) embedding.
  template <typename T, typename E>
  void
  check_composition_algebra() {
    rc::prop("conjugate involution: conjugate(conjugate(z)) == z", [](E z) {
      RC_ASSERT(conjugate(conjugate(z)) == z);
    });

    rc::prop(
      "conjugate anti-automorphism: conjugate(a*b) == "
      "conjugate(b)*conjugate(a)",
      [](E a, E b) {
        RC_ASSERT(conjugate(a * b) == conjugate(b) * conjugate(a));
      });

    rc::prop("norm identity (right): z * conjugate(z) == E(norm(z))", [](E z) {
      RC_ASSERT(z * conjugate(z) == E(norm(z)));
    });

    rc::prop("norm identity (left): conjugate(z) * z == E(norm(z))", [](E z) {
      RC_ASSERT(conjugate(z) * z == E(norm(z)));
    });

    rc::prop(
      "norm multiplicativity: norm(a*b) == norm(a) * norm(b)",
      [](E a, E b) { RC_ASSERT(norm(a * b) == norm(a) * norm(b)); });
  }

} // namespace arithkit::laws
