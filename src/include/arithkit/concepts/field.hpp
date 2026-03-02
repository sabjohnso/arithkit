#pragma once

/// @file field.hpp
/// @brief Field — a commutative DivisionRing, equivalently a
///        CommutativeRing where every nonzero element is invertible.
///
/// Laws (enforced by property tests):
///   All Ring laws + multiplicative commutativity + multiplicative inverse

#include <arithkit/concepts/commutative_ring.hpp>
#include <arithkit/concepts/division_ring.hpp>

namespace arithkit {

  template <typename T>
  concept Field = CommutativeRing<T> && DivisionRing<T>;

} // namespace arithkit
