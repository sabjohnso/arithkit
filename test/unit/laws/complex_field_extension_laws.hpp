#pragma once

/// @file complex_field_extension_laws.hpp
/// @brief Extension laws specific to Complex<T> as the field extension T(i).
///
/// Adds norm-based laws to the ring extension suite: the norm identity
/// z·conj(z)=norm(z), multiplicativity of the norm, and norm vanishes iff zero.

#include <arithkit/complex.hpp>
#include <arithkit/concepts/field.hpp>
#include <arithkit/traits/identity_element.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "complex_ring_extension_laws.hpp"

namespace arithkit::laws {

  template <Field T>
  void
  check_complex_field_extension() {
    using C = Complex<T>;

    check_complex_ring_extension<T>();

    rc::prop("norm identity: z * conj(z) == C(norm(z))", [](C z) {
      RC_ASSERT(z * conjugate(z) == C(norm(z)));
    });

    rc::prop(
      "norm is multiplicative: norm(z*w) == norm(z) * norm(w)",
      [](C z, C w) { RC_ASSERT(norm(z * w) == norm(z) * norm(w)); });

    rc::prop("norm vanishes iff zero: z == 0 <=> norm(z) == 0", [](C z) {
      auto zero_c = identity_v<C, additive_tag>();
      auto zero_t = identity_v<T, additive_tag>();
      RC_ASSERT((z == zero_c) == (norm(z) == zero_t));
    });

    // Mixed scalar/complex division — T is a subfield of Complex<T>

    rc::prop("z / t == z / C(t)", [](C z1, C z2) {
      auto t = z1.real();
      auto zero_t = identity_v<T, additive_tag>();
      RC_PRE(t != zero_t);
      RC_ASSERT(z2 / t == z2 / C(t));
    });

    rc::prop("t / z == C(t) / z", [](C z1, C z2) {
      auto t = z1.real();
      auto zero_c = identity_v<C, additive_tag>();
      RC_PRE(z2 != zero_c);
      RC_ASSERT(t / z2 == C(t) / z2);
    });
  }

} // namespace arithkit::laws
