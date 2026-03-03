#pragma once

/// @file monoid_augmentation.hpp
/// @brief MonoidAugmentation<T,E> — concept for a scalar type T augmented
///        with a dependency index set (union monoid).
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
///   value semantics:    value(a op b) == value(a) op value(b)
///   index union:        indices(a op b) == indices(a) ∪ indices(b)
///   index monotonicity: indices(a) ⊆ indices(a + b)

#include <concepts>

namespace arithkit {

  template <typename T, typename E>
  concept MonoidAugmentation = std::regular<E> && requires(E e, T t) {
    E(t); // embedding ctor: scalar with ∅ index set
    { e.value() } -> std::convertible_to<T>;
    { e.indices() }; // index set accessor (type is unspecified)
    { e + e } -> std::convertible_to<E>;
    { e - e } -> std::convertible_to<E>;
    { e * e } -> std::convertible_to<E>;
    { e / e } -> std::convertible_to<E>;
    { e + t } -> std::convertible_to<E>;
    { t + e } -> std::convertible_to<E>;
    { e - t } -> std::convertible_to<E>;
    { t - e } -> std::convertible_to<E>;
    { e * t } -> std::convertible_to<E>;
    { t * e } -> std::convertible_to<E>;
    { e / t } -> std::convertible_to<E>;
    { t / e } -> std::convertible_to<E>;
  };

} // namespace arithkit
