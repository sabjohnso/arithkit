#pragma once

/// @file alternative_division_algebra.hpp
/// @brief AlternativeDivisionAlgebra — a NonAssociativeRing where every
///        nonzero element has a multiplicative inverse.
///
/// The target concept for octonions. Multiplication is neither
/// commutative nor associative, but satisfies alternativity:
///   left:       a*(a*b) == (a*a)*b
///   right:      (a*b)*b == a*(b*b)
///   flexibility: a*(b*a) == (a*b)*a
///
/// Laws (enforced by property tests):
///   alternativity (left, right), flexibility, multiplicative inverse

#include <arithkit/concepts/non_associative_ring.hpp>

namespace arithkit {

  template <typename T>
  concept AlternativeDivisionAlgebra =
    NonAssociativeRing<T> && requires(T a, T b) {
      { a / b } -> std::convertible_to<T>;
    };

} // namespace arithkit
