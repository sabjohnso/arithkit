#pragma once

/// @file magma_augmentation.hpp
/// @brief MagmaAugmentation<T,E> — the most general augmentation concept.
///
/// E pairs a base scalar type T with an augmentation payload that combines
/// via a magma operation (a binary operation with no further laws).
///
/// Laws (enforced by property tests, see magma_augmentation_laws.hpp):
///   value passthrough: value(a op b) == value(a) op value(b)
///   closure:           indices(a op b) is well-defined

#include <concepts>

namespace arithkit {

  template <typename T, typename E>
  concept MagmaAugmentation = std::regular<E> && requires(E e, T t) {
    E(t); // embedding ctor: scalar with identity payload
    { e.value() } -> std::convertible_to<T>;
    { e.indices() }; // payload accessor (type is unspecified)
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
