#pragma once

/// @file division_ring.hpp
/// @brief DivisionRing — a Ring where every nonzero element has a
///        multiplicative inverse (not necessarily commutative).
///
/// Laws (enforced by property tests):
///   inverse: for a != 0, a * a_inv == 1 && a_inv * a == 1
///
/// Note: Division rings are NOT required to be commutative.
///       Quaternions are a division ring but not a field.

#include <arithkit/concepts/ring.hpp>

namespace arithkit {

  template <typename T>
  concept DivisionRing = Ring<T> && requires(T a, T b) {
    { a / b } -> std::convertible_to<T>;
  };

} // namespace arithkit
