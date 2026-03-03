#pragma once

/// @file multiplicative_monoid.hpp
/// @brief MultiplicativeMonoid — a MultiplicativeSemigroup with a
///        multiplicative identity.
///
/// Laws (enforced by property tests):
///   associativity: (a * b) * c == a * (b * c)
///   identity: a * 1 == a && 1 * a == a
///
/// Equivalent to MultiplicativeSemigroup && MultiplicativeUnitalMagma.

#include <arithkit/concepts/multiplicative_semigroup.hpp>
#include <arithkit/concepts/multiplicative_unital_magma.hpp>

namespace arithkit {

  template <typename T>
  concept MultiplicativeMonoid =
    MultiplicativeSemigroup<T> && MultiplicativeUnitalMagma<T>;

} // namespace arithkit
