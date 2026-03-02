#pragma once

/// @file approx_equal.hpp
/// @brief Approximate equality for property tests.

#include <cmath>
#include <concepts>

namespace arithkit::laws {

  /// Approximate equality for std::floating_point types.
  template <std::floating_point T>
  bool
  approx_equal(T a, T b, T rel_tol = T(1e-9), T abs_tol = T(1e-12)) {
    if (a == b) return true;
    auto diff = std::abs(a - b);
    auto mag = std::max(std::abs(a), std::abs(b));
    return diff <= abs_tol || diff <= rel_tol * mag;
  }

  /// Approximate equality for non-floating-point ordered types
  /// that support arithmetic (e.g. ArbitraryFloat).
  /// Uses relative tolerance of ~10^-4 (generous for 34-digit
  /// precision).
  template <typename T>
    requires(!std::floating_point<T>)
  bool
  approx_equal(const T& a, const T& b) {
    if (a == b) return true;

    // Compute absolute difference
    auto diff = a - b;
    if (diff > T{}) {
      // diff is positive
    } else {
      diff = -diff; // make positive
    }

    // Compute magnitude
    auto abs_a = a > T{} ? a : -a;
    auto abs_b = b > T{} ? b : -b;
    auto mag = abs_a > abs_b ? abs_a : abs_b;

    if (mag == T{}) return diff == T{};

    // diff < mag / 10000  (about 4 digits of tolerance)
    return diff * T(10000) < mag;
  }

} // namespace arithkit::laws
