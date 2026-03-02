#pragma once

/// @file multiplicative_magma.hpp
/// @brief MultiplicativeMagma — a type with a binary * operation.
///
/// Laws: closure (guaranteed by the type system for same-type *).

#include <concepts>

namespace arithkit {

  template <typename T>
  concept MultiplicativeMagma = std::regular<T> && requires(T a, T b) {
    { a * b } -> std::convertible_to<T>;
  };

} // namespace arithkit
