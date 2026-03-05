#pragma once

/// @file magma_augmentation_laws.hpp
/// @brief Laws for MagmaAugmentation: value passthrough and closure.
///
/// These are the minimal structural invariants for any augmentation:
///   1. Value passthrough: value(a op b) == value(a) op value(b)
///   2. Closure: indices(a op b) is well-defined (guaranteed by concepts)

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

namespace arithkit::laws {

  /// Check the value-passthrough law for a MagmaAugmentation.
  ///
  /// Requires rc::Arbitrary<E> to be defined by the calling translation
  /// unit. T must support +, -, *, /, default construction, and operator!=.
  template <typename T, typename E>
  void
  check_magma_augmentation() {
    rc::prop(
      "magma aug value +: value(a+b) == value(a)+value(b)",
      [](E a, E b) { RC_ASSERT((a + b).value() == a.value() + b.value()); });

    rc::prop(
      "magma aug value -: value(a-b) == value(a)-value(b)",
      [](E a, E b) { RC_ASSERT((a - b).value() == a.value() - b.value()); });

    rc::prop(
      "magma aug value *: value(a*b) == value(a)*value(b)",
      [](E a, E b) { RC_ASSERT((a * b).value() == a.value() * b.value()); });

    rc::prop(
      "magma aug value /: value(a/b) == value(a)/value(b)", [](E a, E b) {
        RC_PRE(b.value() != T{});
        RC_ASSERT((a / b).value() == a.value() / b.value());
      });

    // --- Mixed scalar/augmented operator laws ---

    rc::prop("magma aug mixed +: z + t == z + E(t)", [](E z, E s) {
      auto t = s.value();
      RC_ASSERT(z + t == z + E(t));
    });

    rc::prop("magma aug mixed +: t + z == E(t) + z", [](E z, E s) {
      auto t = s.value();
      RC_ASSERT(t + z == E(t) + z);
    });

    rc::prop("magma aug mixed -: z - t == z - E(t)", [](E z, E s) {
      auto t = s.value();
      RC_ASSERT(z - t == z - E(t));
    });

    rc::prop("magma aug mixed -: t - z == E(t) - z", [](E z, E s) {
      auto t = s.value();
      RC_ASSERT(t - z == E(t) - z);
    });

    rc::prop("magma aug mixed *: z * t == z * E(t)", [](E z, E s) {
      auto t = s.value();
      RC_ASSERT(z * t == z * E(t));
    });

    rc::prop("magma aug mixed *: t * z == E(t) * z", [](E z, E s) {
      auto t = s.value();
      RC_ASSERT(t * z == E(t) * z);
    });

    rc::prop("magma aug mixed /: z / t == z / E(t)", [](E z, E s) {
      auto t = s.value();
      RC_PRE(t != T{});
      RC_ASSERT(z / t == z / E(t));
    });

    rc::prop("magma aug mixed /: t / z == E(t) / z", [](E z, E s) {
      auto t = s.value();
      RC_PRE(z.value() != T{});
      RC_ASSERT(t / z == E(t) / z);
    });
  }

} // namespace arithkit::laws
