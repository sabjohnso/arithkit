#pragma once

/// @file multiplicative_semigroup.hpp
/// @brief MultiplicativeSemigroup — a MultiplicativeMagma where * is
///        associative.
///
/// Laws (enforced by property tests):
///   associativity: (a * b) * c == a * (b * c)

#include <arithkit/concepts/multiplicative_magma.hpp>

namespace arithkit {

  template <typename T>
  concept MultiplicativeSemigroup = MultiplicativeMagma<T>;

} // namespace arithkit
