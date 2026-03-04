/// @file fixed_point_filter.cpp
/// @brief IIR low-pass filter using fixed-point arithmetic.
///
/// Demonstrates FixedPoint<16,16> in a classic embedded DSP use case:
/// a simple first-order IIR (infinite impulse response) low-pass
/// filter applied to a noisy square wave signal.
///
/// Filter equation:  y[n] = alpha * x[n] + (1 - alpha) * y[n-1]

#include <cmath>
#include <iomanip>
#include <iostream>

#include <arithkit/fixed_point.hpp>

using arithkit::FixedPoint;
using FP = FixedPoint<16, 16>;

int main() {
  std::cout << "=== Fixed-Point IIR Low-Pass Filter ===" << '\n';
  std::cout << "Filter: y[n] = alpha * x[n] + (1-alpha) * y[n-1]"
            << '\n';

  // Filter coefficient (smoothing factor)
  auto alpha = FP(0.1);
  auto one_minus_alpha = FP(1.0) - alpha;

  std::cout << "alpha = " << alpha << '\n';
  std::cout << "1 - alpha = " << one_minus_alpha << '\n' << '\n';

  // Generate a noisy square wave as input signal
  constexpr int samples = 40;
  auto y = FP(0.0); // filter state

  std::cout << std::fixed << std::setprecision(4);
  std::cout << std::setw(6) << "n" << std::setw(12) << "x[n]"
            << std::setw(12) << "y[n]" << '\n';
  std::cout << std::string(30, '-') << '\n';

  for (int n = 0; n < samples; ++n) {
    // Square wave: +1 for first half, -1 for second half
    auto x = (n < samples / 2) ? FP(1.0) : FP(-1.0);

    // Add some "noise" (small perturbation)
    auto noise = FP(0.1 * std::sin(n * 0.7));
    x = x + noise;

    // Apply IIR low-pass filter
    y = alpha * x + one_minus_alpha * y;

    std::cout << std::setw(6) << n << std::setw(12) << x.to_double()
              << std::setw(12) << y.to_double() << '\n';
  }

  std::cout << '\n'
            << "Note: the filter smooths out the noisy square wave,"
            << '\n'
            << "gradually rising toward +1 then falling toward -1."
            << '\n';

  return 0;
}
