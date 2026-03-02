#pragma once

/// @file multiplicative_monoid.hpp
/// @brief MultiplicativeMonoid — a MultiplicativeSemigroup with a
///        multiplicative identity.
///
/// Laws (enforced by property tests):
///   identity: a * 1 == a && 1 * a == a
///
/// The identity element is provided by
/// identity_element<T, multiplicative_tag>.

#include <arithkit/concepts/multiplicative_semigroup.hpp>
#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  template <typename T>
  concept MultiplicativeMonoid = MultiplicativeSemigroup<T> && requires {
    {
      identity_element<T, multiplicative_tag>::make()
    } -> std::convertible_to<T>;
  };

} // namespace arithkit
