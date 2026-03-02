#pragma once

/// @file commutative_ring.hpp
/// @brief CommutativeRing — a Ring where * is commutative.
///
/// Laws (enforced by property tests):
///   multiplicative commutativity: a * b == b * a

#include <arithkit/concepts/ring.hpp>

namespace arithkit {

  template <typename T>
  concept CommutativeRing = Ring<T>;

} // namespace arithkit
