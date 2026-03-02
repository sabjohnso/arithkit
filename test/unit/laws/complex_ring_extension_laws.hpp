#pragma once

/// @file complex_ring_extension_laws.hpp
/// @brief Extension laws specific to Complex<T> as a ring T[i]/(i²+1).
///
/// These properties are structural laws of the complex extension that go beyond
/// the generic ring axioms: the defining relation i²=−1, the embedding t↦(t,0)
/// as a ring homomorphism, and conjugation as a ring involution.

#include <arithkit/complex.hpp>
#include <arithkit/concepts/ring.hpp>
#include <arithkit/traits/identity_element.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

namespace arithkit::laws {

  template <Ring T>
  void
  check_complex_ring_extension() {
    using C = Complex<T>;

    rc::prop("defining relation: i*i == -1", []() {
      auto zero = identity_v<T, additive_tag>();
      auto one = identity_v<T, multiplicative_tag>();
      C i_unit(zero, one);
      RC_ASSERT(i_unit * i_unit == C(-one, zero));
    });

    rc::prop(
      "embedding preserves addition: C(a+b) == C(a) + C(b)", [](C z1, C z2) {
        auto a = z1.real();
        auto b = z2.real();
        RC_ASSERT(C(a + b) == C(a) + C(b));
      });

    rc::prop(
      "embedding preserves multiplication: C(a*b) == C(a) * C(b)",
      [](C z1, C z2) {
        auto a = z1.real();
        auto b = z2.real();
        RC_ASSERT(C(a * b) == C(a) * C(b));
      });

    rc::prop(
      "conjugation is additive: conj(a+b) == conj(a) + conj(b)", [](C a, C b) {
        RC_ASSERT(conjugate(a + b) == conjugate(a) + conjugate(b));
      });

    rc::prop(
      "conjugation is multiplicative: conj(a*b) == conj(a) * conj(b)",
      [](C a, C b) {
        RC_ASSERT(conjugate(a * b) == conjugate(a) * conjugate(b));
      });

    rc::prop("conjugation is involutive: conj(conj(z)) == z", [](C z) {
      RC_ASSERT(conjugate(conjugate(z)) == z);
    });
  }

} // namespace arithkit::laws
