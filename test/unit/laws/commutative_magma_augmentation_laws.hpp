#pragma once

/// @file commutative_magma_augmentation_laws.hpp
/// @brief Laws for CommutativeMagmaAugmentation: magma laws + commutativity
///        of the payload combining operation.
///
/// Additional law beyond MagmaAugmentation:
///   commutativity: indices(a op b) == indices(b op a)

#include "magma_augmentation_laws.hpp"

namespace arithkit::laws {

  /// Check CommutativeMagmaAugmentation laws.
  ///
  /// Calls check_magma_augmentation<T,E>() then adds commutativity tests
  /// for the payload combining operation.
  template <typename T, typename E>
  void
  check_commutative_magma_augmentation() {
    check_magma_augmentation<T, E>();

    rc::prop("comm magma aug +: indices(a+b) == indices(b+a)", [](E a, E b) {
      RC_ASSERT((a + b).indices() == (b + a).indices());
    });

    rc::prop("comm magma aug -: indices(a-b) == indices(b-a)", [](E a, E b) {
      RC_ASSERT((a - b).indices() == (b - a).indices());
    });

    rc::prop("comm magma aug *: indices(a*b) == indices(b*a)", [](E a, E b) {
      RC_ASSERT((a * b).indices() == (b * a).indices());
    });

    rc::prop("comm magma aug /: indices(a/b) == indices(b/a)", [](E a, E b) {
      RC_PRE(a.value() != T{});
      RC_PRE(b.value() != T{});
      RC_ASSERT((a / b).indices() == (b / a).indices());
    });
  }

} // namespace arithkit::laws
