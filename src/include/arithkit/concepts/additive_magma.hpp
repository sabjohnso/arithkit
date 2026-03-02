#pragma once

/// @file additive_magma.hpp
/// @brief AdditiveMagma concept — a type with a binary + operation.
///
/// Laws: closure (guaranteed by the type system for same-type +).

#include <concepts>

namespace arithkit {

  template <typename T>
  concept AdditiveMagma = std::regular<T> && requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
  };

} // namespace arithkit
