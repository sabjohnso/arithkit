/// @file compute_pi.cpp
/// @brief Computing pi to many digits using ArbitraryFloat.
///
/// Uses Machin's formula:  pi/4 = 4*arctan(1/5) - arctan(1/239)
/// with a Taylor series for arctan evaluated at high precision.
///
/// The ArbitraryFloat type uses a BigInt mantissa * 10^exponent,
/// so precision is limited only by memory and patience.

#include <iostream>

#include <arithkit/arbitrary_float.hpp>

using arithkit::ArbitraryFloat;
using arithkit::BigInt;

auto
make_af(long long val, int precision) -> ArbitraryFloat {
  return ArbitraryFloat(BigInt(val), 0, precision);
}

/// Compute arctan(1/k) to the given precision using Taylor series:
/// arctan(1/k) = 1/k - 1/(3*k^3) + 1/(5*k^5) - ...
///
/// For k=5: converges in ~precision/1.4 terms
/// For k=239: converges in ~precision/4.8 terms
auto
arctan_recip(long long k, int precision) -> ArbitraryFloat {
  auto one = make_af(1, precision);
  auto kk = make_af(k, precision);
  auto k_sq = kk * kk;
  auto zero = make_af(0, precision);

  auto power = one / kk;
  auto result = power;

  // Maximum terms needed: precision / log10(k^2) + safety margin
  int max_terms = precision + 10;

  for (int n = 1; n < max_terms; ++n) {
    power = power / k_sq;
    auto denom = make_af(2 * n + 1, precision);
    auto term = power / denom;

    if (term == zero) break;

    if (n % 2 == 1) {
      result = result - term;
    } else {
      result = result + term;
    }
  }

  return result;
}

int
main() {
  constexpr int precision = 34;

  std::cout << "=== Computing Pi with ArbitraryFloat ===\n\n";
  std::cout << "Precision: " << precision << " significant digits\n";
  std::cout << "Formula:   pi/4 = 4*arctan(1/5) - arctan(1/239)  (Machin)\n\n";

  auto four = make_af(4, precision);

  std::cout << "Computing arctan(1/5)...\n";
  auto atan_5 = arctan_recip(5, precision);

  std::cout << "Computing arctan(1/239)...\n";
  auto atan_239 = arctan_recip(239, precision);

  auto pi = four * (four * atan_5 - atan_239);

  std::cout << "\npi = " << pi << "\n\n";
  std::cout << "Reference:\n"
            << "3.14159265358979323846264338327950288419716939937510...\n\n";
  std::cout << "ArbitraryFloat carries " << precision
            << " significant decimal digits\n"
            << "using a BigInt mantissa — far beyond hardware float's ~15.\n";

  return 0;
}
