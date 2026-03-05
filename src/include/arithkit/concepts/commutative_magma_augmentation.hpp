#pragma once

/// @file commutative_magma_augmentation.hpp
/// @brief CommutativeMagmaAugmentation<T,E> — a MagmaAugmentation where the
///        payload combining operation is commutative.
///
/// Laws (enforced by property tests, see
///       commutative_magma_augmentation_laws.hpp):
///   All MagmaAugmentation laws, plus:
///   commutativity: indices(a op b) == indices(b op a)

#include <arithkit/concepts/magma_augmentation.hpp>

namespace arithkit {

  template <typename T, typename E>
  concept CommutativeMagmaAugmentation = MagmaAugmentation<T, E>;

} // namespace arithkit
