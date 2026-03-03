#pragma once

/// @file multiplicative_unital_magma.hpp
/// @brief MultiplicativeUnitalMagma — a MultiplicativeMagma with a
///        multiplicative identity (no associativity required).
///
/// Laws (enforced by property tests):
///   identity: a * 1 == a && 1 * a == a
///
/// This concept extracts the identity requirement from
/// MultiplicativeMonoid without demanding associativity.
/// Octonions satisfy this but not MultiplicativeMonoid.

#include <arithkit/concepts/multiplicative_magma.hpp>
#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  template <typename T>
  concept MultiplicativeUnitalMagma = MultiplicativeMagma<T> && requires {
    {
      identity_element<T, multiplicative_tag>::make()
    } -> std::convertible_to<T>;
  };

} // namespace arithkit
