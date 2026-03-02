#pragma once

/// @file module.hpp
/// @brief Module<R,M> — an R-module: M is an abelian group under +,
///        with scalar multiplication by elements of ring R.
///
/// Laws (enforced by property tests):
///   scalar distributivity:  r * (m1 + m2) == r*m1 + r*m2
///   ring distributivity:    (r1 + r2) * m == r1*m + r2*m
///   associativity:          (r1 * r2) * m == r1 * (r2 * m)
///   identity:               1 * m == m

#include <arithkit/concepts/additive_abelian_group.hpp>
#include <arithkit/concepts/ring.hpp>

namespace arithkit {

  template <typename R, typename M>
  concept Module = Ring<R> && AdditiveAbelianGroup<M> && requires(R r, M m) {
    { r * m } -> std::convertible_to<M>;
  };

} // namespace arithkit
