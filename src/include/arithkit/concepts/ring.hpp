#pragma once

/// @file ring.hpp
/// @brief Ring — an AdditiveAbelianGroup + MultiplicativeMonoid where *
///        distributes over +.
///
/// Laws (enforced by property tests):
///   left distributivity:  a * (b + c) == a*b + a*c
///   right distributivity: (a + b) * c == a*c + b*c

#include <arithkit/concepts/additive_abelian_group.hpp>
#include <arithkit/concepts/multiplicative_monoid.hpp>

namespace arithkit {

  template <typename T>
  concept Ring = AdditiveAbelianGroup<T> && MultiplicativeMonoid<T>;

} // namespace arithkit
