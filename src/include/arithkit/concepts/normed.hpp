#pragma once

/// @file normed.hpp
/// @brief Normed — a type with a norm function returning a non-negative
///        value.
///
/// Laws (enforced by property tests):
///   non-negativity:     norm(a) >= 0
///   definiteness:       norm(a) == 0  iff  a == 0
///   triangle inequality: norm(a + b) <= norm(a) + norm(b)

#include <concepts>

namespace arithkit {

  template <typename T>
  concept Normed = std::regular<T> && requires(T a) {
    { norm(a) };
  };

} // namespace arithkit
