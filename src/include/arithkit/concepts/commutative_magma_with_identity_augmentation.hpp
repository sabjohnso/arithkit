#pragma once

/// @file commutative_magma_with_identity_augmentation.hpp
/// @brief CommutativeMagmaWithIdentityAugmentation<T,E> — a
///        CommutativeMagmaAugmentation where the payload has an identity
///        element.
///
/// The identity element is the payload of embedded scalars: indices(E(t))
/// is the identity for the payload combining operation.
///
/// Laws (enforced by property tests, see
///       commutative_magma_with_identity_augmentation_laws.hpp):
///   All CommutativeMagmaAugmentation laws, plus:
///   identity: indices(E(t)) is the payload identity
///             indices(a op E(t)) == indices(a)

#include <arithkit/concepts/commutative_magma_augmentation.hpp>

namespace arithkit {

  template <typename T, typename E>
  concept CommutativeMagmaWithIdentityAugmentation =
    CommutativeMagmaAugmentation<T, E>;

} // namespace arithkit
