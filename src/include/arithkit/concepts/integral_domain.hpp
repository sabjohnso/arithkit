#pragma once

/// @file integral_domain.hpp
/// @brief IntegralDomain — a CommutativeRing with no zero divisors.
///
/// Laws (enforced by property tests):
///   no zero divisors: a * b == 0  implies  a == 0 || b == 0

#include <arithkit/concepts/commutative_ring.hpp>

namespace arithkit {

  template <typename T>
  concept IntegralDomain = CommutativeRing<T>;

} // namespace arithkit
