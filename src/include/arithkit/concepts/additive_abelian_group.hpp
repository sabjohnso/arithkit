#pragma once

/// @file additive_abelian_group.hpp
/// @brief AdditiveAbelianGroup — an AdditiveGroup where + is commutative.
///
/// Laws (enforced by property tests):
///   commutativity: a + b == b + a

#include <arithkit/concepts/additive_group.hpp>

namespace arithkit {

  template <typename T>
  concept AdditiveAbelianGroup = AdditiveGroup<T>;

} // namespace arithkit
