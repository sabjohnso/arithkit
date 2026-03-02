#pragma once

/// @file additive_semigroup.hpp
/// @brief AdditiveSemigroup — an AdditiveMagma where + is associative.
///
/// Laws (enforced by property tests, not the type system):
///   associativity: (a + b) + c == a + (b + c)

#include <arithkit/concepts/additive_magma.hpp>

namespace arithkit {

  template <typename T>
  concept AdditiveSemigroup = AdditiveMagma<T>;

} // namespace arithkit
