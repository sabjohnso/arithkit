#pragma once

/// @file additive_monoid.hpp
/// @brief AdditiveMonoid — an AdditiveSemigroup with an additive identity.
///
/// Laws (enforced by property tests):
///   identity: a + 0 == a && 0 + a == a
///
/// The identity element is provided by identity_element<T, additive_tag>.

#include <arithkit/concepts/additive_semigroup.hpp>
#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  template <typename T>
  concept AdditiveMonoid = AdditiveSemigroup<T> && requires {
    { identity_element<T, additive_tag>::make() } -> std::convertible_to<T>;
  };

} // namespace arithkit
