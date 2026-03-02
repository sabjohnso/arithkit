#pragma once

/// @file algebra.hpp
/// @brief Algebra<R,A> — an R-algebra: A is both an R-module and a Ring,
///        with scalar multiplication compatible with ring multiplication.
///
/// Laws (enforced by property tests):
///   all Module<R,A> laws
///   all Ring<A> laws
///   compatibility: r * (a1 * a2) == (r * a1) * a2 == a1 * (r * a2)

#include <arithkit/concepts/module.hpp>

namespace arithkit {

  template <typename R, typename A>
  concept Algebra = Module<R, A> && Ring<A>;

} // namespace arithkit
