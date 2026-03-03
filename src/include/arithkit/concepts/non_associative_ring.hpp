#pragma once

/// @file non_associative_ring.hpp
/// @brief NonAssociativeRing — an AdditiveAbelianGroup with a
///        MultiplicativeUnitalMagma where * distributes over +.
///
/// Like Ring but without requiring multiplicative associativity.
/// Ring automatically refines NonAssociativeRing because
/// MultiplicativeMonoid => MultiplicativeUnitalMagma.
///
/// Laws (enforced by property tests):
///   left distributivity:  a * (b + c) == a*b + a*c
///   right distributivity: (a + b) * c == a*c + b*c

#include <arithkit/concepts/additive_abelian_group.hpp>
#include <arithkit/concepts/multiplicative_unital_magma.hpp>

namespace arithkit {

  template <typename T>
  concept NonAssociativeRing =
    AdditiveAbelianGroup<T> && MultiplicativeUnitalMagma<T>;

} // namespace arithkit
