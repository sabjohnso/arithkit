#pragma once

/// @file additive_group.hpp
/// @brief AdditiveGroup — an AdditiveMonoid with additive inverses.
///
/// Laws (enforced by property tests):
///   inverse: a + (-a) == 0 && (-a) + a == 0

#include <arithkit/concepts/additive_monoid.hpp>

namespace arithkit {

  template <typename T>
  concept AdditiveGroup = AdditiveMonoid<T> && requires(T a) {
    { -a } -> std::convertible_to<T>;
    { a - a } -> std::convertible_to<T>;
  };

} // namespace arithkit
