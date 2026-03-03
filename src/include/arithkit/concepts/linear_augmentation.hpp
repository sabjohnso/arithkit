#pragma once

/// @file linear_augmentation.hpp
/// @brief LinearAugmentation<T,E> and LinearAugmentationField<T,E> —
///        concepts for T[ε]/(ε²) augmented rings.
///
/// LinearAugmentation: E is a commutative ring extending base ring T
/// by a nilpotent element ε (ε² = 0).  Dual<T> is the canonical model.
///
/// LinearAugmentationField: refinement where the base T is also a Field.
///
/// Laws (enforced by property tests, see linear_augmentation_laws.hpp):
///   embedding:        E(t).real() == t, E(t).dual() == 0
///   projection:       E(a, b).real() == a, E(a, b).dual() == b
///   mixed arithmetic: E(t) op e == E(t) op e (scalar as embedded dual)

#include <arithkit/concepts/commutative_ring.hpp>
#include <arithkit/concepts/field.hpp>

namespace arithkit {

  template <typename T, typename E>
  concept LinearAugmentation =
    CommutativeRing<T> && CommutativeRing<E> && requires(E e, T t) {
      E(t);    // embedding ctor: scalar → dual with zero ε-part
      E(t, t); // full ctor: explicit real and dual parts
      { e.real() } -> std::convertible_to<T>;
      { e.dual() } -> std::convertible_to<T>;
      { e + t } -> std::convertible_to<E>;
      { t + e } -> std::convertible_to<E>;
      { e - t } -> std::convertible_to<E>;
      { t - e } -> std::convertible_to<E>;
      { e * t } -> std::convertible_to<E>;
      { t * e } -> std::convertible_to<E>;
    };

  template <typename T, typename E>
  concept LinearAugmentationField = Field<T> && LinearAugmentation<T, E>;

} // namespace arithkit
