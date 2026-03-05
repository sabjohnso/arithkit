#pragma once

/// @file monoid_augmentation.hpp
/// @brief MonoidAugmentation<T,E> — concept for a scalar type T augmented
///        with a dependency index set (union monoid).
///
/// Refines CommutativeMagmaWithIdentityAugmentation by adding the semantic
/// requirement of associativity on the payload combining operation,
/// completing the commutative monoid structure.
///
/// The index set tracks which variables a computation depends on,
/// propagated by union across all four arithmetic operations:
///   (v1,S1) op (v2,S2) = (v1 op v2, S1 ∪ S2)
///
/// T is intentionally unconstrained at the concept level — Sparsity<T>
/// itself is parameterized on any T, and the law suite follows suit.
///
/// Sparsity<T> is the canonical model.
///
/// Laws (enforced by property tests, see monoid_augmentation_laws.hpp):
///   All CommutativeMagmaWithIdentityAugmentation laws, plus:
///   associativity:      indices((a op b) op c) == indices(a op (b op c))
///   index union:        indices(a op b) == indices(a) ∪ indices(b)
///   index monotonicity: indices(a) ⊆ indices(a + b)

#include <arithkit/concepts/commutative_magma_with_identity_augmentation.hpp>

namespace arithkit {

  template <typename T, typename E>
  concept MonoidAugmentation = CommutativeMagmaWithIdentityAugmentation<T, E>;

} // namespace arithkit
