#pragma once

/// @file commutative_magma_with_identity_augmentation_laws.hpp
/// @brief Laws for CommutativeMagmaWithIdentityAugmentation: commutative
///        magma laws + payload identity element.
///
/// The payload identity is the payload of embedded scalars.
/// Additional law beyond CommutativeMagmaAugmentation:
///   identity: indices(a op E(t)) == indices(a)
///             indices(E(t)) is the payload identity (empty set for Sparsity)

#include "commutative_magma_augmentation_laws.hpp"

namespace arithkit::laws {

  /// Check CommutativeMagmaWithIdentityAugmentation laws.
  ///
  /// Calls check_commutative_magma_augmentation<T,E>() then adds identity
  /// tests for the payload combining operation.
  template <typename T, typename E>
  void
  check_commutative_magma_with_identity_augmentation() {
    check_commutative_magma_augmentation<T, E>();

    // Embedding produces the identity payload
    rc::prop(
      "identity aug embedding: indices(E(t)) is identity payload", [](E a) {
        auto t = a.value();
        RC_ASSERT(E(t).indices().empty());
      });

    // Identity payload is absorbed by the combining operation
    rc::prop("identity aug +: indices(a + E(t)) == indices(a)", [](E a, E b) {
      auto t = b.value();
      RC_ASSERT((a + E(t)).indices() == a.indices());
    });

    rc::prop("identity aug +: indices(E(t) + a) == indices(a)", [](E a, E b) {
      auto t = b.value();
      RC_ASSERT((E(t) + a).indices() == a.indices());
    });

    rc::prop("identity aug -: indices(a - E(t)) == indices(a)", [](E a, E b) {
      auto t = b.value();
      RC_ASSERT((a - E(t)).indices() == a.indices());
    });

    rc::prop("identity aug *: indices(a * E(t)) == indices(a)", [](E a, E b) {
      auto t = b.value();
      RC_ASSERT((a * E(t)).indices() == a.indices());
    });

    rc::prop("identity aug /: indices(a / E(t)) == indices(a)", [](E a, E b) {
      auto t = b.value();
      RC_PRE(t != T{});
      RC_ASSERT((a / E(t)).indices() == a.indices());
    });
  }

} // namespace arithkit::laws
