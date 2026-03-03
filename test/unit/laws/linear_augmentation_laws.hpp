#pragma once

/// @file linear_augmentation_laws.hpp
/// @brief Structural laws for Dual<T> as the linear augmentation T[ε]/(ε²).
///
/// These laws capture the square-zero ideal structure and the ring-homomorphism
/// embedding T → Dual<T> that go beyond the generic CommutativeRing axioms.
/// The field extension adds an invertibility formula for Dual<T> when T is a
/// Field.

#include <arithkit/concepts/commutative_ring.hpp>
#include <arithkit/concepts/field.hpp>
#include <arithkit/dual.hpp>
#include <arithkit/traits/identity_element.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "approx_equal.hpp"

namespace arithkit::laws {

  /// Check the five structural laws of the linear augmentation ring T[ε]/(ε²).
  ///
  /// Requires rc::Arbitrary<Dual<T>> to be defined by the calling translation
  /// unit.
  template <CommutativeRing T>
  void
  check_linear_augmentation_ring() {
    using E = Dual<T>;

    rc::prop("square-zero ideal: (0,v)*(0,w) == 0", [](E z1, E z2) {
      auto zero = identity_v<T, additive_tag>();
      E a(zero, z1.dual());
      E b(zero, z2.dual());
      RC_ASSERT(a * b == E());
    });

    rc::prop(
      "product base: (z1*z2).real() == z1.real()*z2.real()",
      [](E z1, E z2) { RC_ASSERT((z1 * z2).real() == z1.real() * z2.real()); });

    rc::prop(
      "bilinear augmentation: (z1*z2).dual() == "
      "z1.real()*z2.dual() + z2.real()*z1.dual()",
      [](E z1, E z2) {
        RC_ASSERT(
          (z1 * z2).dual() == z1.real() * z2.dual() + z2.real() * z1.dual());
      });

    rc::prop("embedding +: E(a+b) == E(a) + E(b)", [](E z1, E z2) {
      auto a = z1.real();
      auto b = z2.real();
      RC_ASSERT(E(a + b) == E(a) + E(b));
    });

    rc::prop("embedding *: E(a*b) == E(a) * E(b)", [](E z1, E z2) {
      auto a = z1.real();
      auto b = z2.real();
      RC_ASSERT(E(a * b) == E(a) * E(b));
    });

    // --- Mixed scalar/dual operator laws (T embeds as subring t ↦ (t,0)) ---

    rc::prop("mixed +: z + t == z + E(t)", [](E z1, E z2) {
      auto t = z2.real();
      RC_ASSERT(z1 + t == z1 + E(t));
    });

    rc::prop("mixed +: t + z == E(t) + z", [](E z1, E z2) {
      auto t = z2.real();
      RC_ASSERT(t + z1 == E(t) + z1);
    });

    rc::prop("mixed -: z - t == z - E(t)", [](E z1, E z2) {
      auto t = z2.real();
      RC_ASSERT(z1 - t == z1 - E(t));
    });

    rc::prop("mixed -: t - z == E(t) - z", [](E z1, E z2) {
      auto t = z2.real();
      RC_ASSERT(t - z1 == E(t) - z1);
    });

    rc::prop("mixed *: z * t == z * E(t)", [](E z1, E z2) {
      auto t = z2.real();
      RC_ASSERT(z1 * t == z1 * E(t));
    });

    rc::prop("mixed *: t * z == E(t) * z", [](E z1, E z2) {
      auto t = z2.real();
      RC_ASSERT(t * z1 == E(t) * z1);
    });
  }

  /// Check the invertibility law specific to the linear augmentation field.
  ///
  /// Calls check_linear_augmentation_ring<T>() then adds:
  ///   Dual(a,v) * Dual(1/a, -v/a²) == (1,0)   when a ≠ 0
  ///
  /// Mathematical verification:
  ///   Dual(a,v) * Dual(1/a, -v/a²)
  ///   = Dual(a*(1/a), a*(-v/a²) + v*(1/a))
  ///   = Dual(1, -v/a + v/a)
  ///   = Dual(1, 0) ✓
  template <Field T>
  void
  check_linear_augmentation_field() {
    using E = Dual<T>;

    check_linear_augmentation_ring<T>();

    rc::prop(
      "invertibility: Dual(a,v) * Dual(1/a,-v/a²) == (1,0)", [](E z1, E z2) {
        auto a = z1.real();
        auto v = z2.real();
        auto zero = identity_v<T, additive_tag>();
        auto one = identity_v<T, multiplicative_tag>();
        RC_PRE(a != zero);
        auto inv_a = one / a;
        auto inv_du = -(v / (a * a));
        RC_ASSERT(E(a, v) * E(inv_a, inv_du) == E(one, zero));
      });
  }

  /// Approximate variant of check_linear_augmentation_field for
  /// floating-point base scalars where exact equality fails due to rounding.
  ///
  /// Ring laws are exact (integer-valued generators), so this delegates to
  /// check_linear_augmentation_ring<T>() unchanged. Only the invertibility
  /// law uses approximate component-wise comparison.
  template <Field T>
    requires std::floating_point<T>
  void
  check_approx_linear_augmentation_field() {
    using E = Dual<T>;

    check_linear_augmentation_ring<T>();

    rc::prop(
      "approx invertibility: Dual(a,v) * Dual(1/a,-v/a²) ≈ (1,0)",
      [](E z1, E z2) {
        auto a = z1.real();
        auto v = z2.real();
        auto zero = identity_v<T, additive_tag>();
        auto one = identity_v<T, multiplicative_tag>();
        RC_PRE(a != zero);
        auto inv_a = one / a;
        auto inv_du = -(v / (a * a));
        auto result = E(a, v) * E(inv_a, inv_du);
        RC_ASSERT(approx_equal(result.real(), one));
        RC_ASSERT(approx_equal(result.dual(), zero));
      });
  }

} // namespace arithkit::laws
