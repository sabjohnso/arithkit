#pragma once

/// @file interval_arithmetic.hpp
/// @brief IntervalArithmetic — custom concept for interval and affine
///        types that are NOT rings (due to subdistributivity and the
///        dependency problem) but support arithmetic operations.
///
/// Laws (enforced by property tests):
///   containment: if x in X and y in Y, then x+y in X+Y, x*y in X*Y
///   monotone inclusion: X subset Y  implies  f(X) subset f(Y)
///   subdistributivity:  X*(Y+Z) subset X*Y + X*Z  (inclusion, not
///   equality)

#include <concepts>

namespace arithkit {

  template <typename T>
  concept IntervalArithmetic = std::regular<T> && requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
    { a - b } -> std::convertible_to<T>;
    { a * b } -> std::convertible_to<T>;
    { a / b } -> std::convertible_to<T>;
  };

} // namespace arithkit
