#pragma once

/// @file euclidean_domain.hpp
/// @brief EuclideanDomain — an IntegralDomain with a Euclidean division.
///
/// Laws (enforced by property tests):
///   division: for a, b with b != 0, there exist q, r such that
///             a == b*q + r and (r == 0 or norm(r) < norm(b))
///
/// Requires / and % operators for the Euclidean division.

#include <arithkit/concepts/integral_domain.hpp>

namespace arithkit {

  template <typename T>
  concept EuclideanDomain = IntegralDomain<T> && requires(T a, T b) {
    { a / b } -> std::convertible_to<T>;
    { a % b } -> std::convertible_to<T>;
  };

} // namespace arithkit
