/// @file combinatorics.cpp
/// @brief Exact binomial coefficients using BigInt.
///
/// Computes the full row C(100, k) for k = 0..100 and verifies
/// Pascal's identity: C(n,k) = C(n-1,k-1) + C(n-1,k).
///
/// Standard int64 overflows around C(100,50) ~ 10^29.  BigInt
/// handles this exactly with no loss of precision.

#include <iostream>
#include <vector>

#include <arithkit/bigint.hpp>

using arithkit::BigInt;

auto
binomial_row(int n) -> std::vector<BigInt> {
  std::vector<BigInt> row(static_cast<std::size_t>(n) + 1, BigInt(0));
  row[0] = BigInt(1);
  for (int i = 1; i <= n; ++i) {
    for (int j = i; j >= 1; --j) {
      row[static_cast<std::size_t>(j)] =
        row[static_cast<std::size_t>(j)] +
        row[static_cast<std::size_t>(j) - 1];
    }
  }
  return row;
}

int
main() {
  constexpr int n = 100;

  std::cout << "=== Exact Binomial Coefficients with BigInt ===\n\n";

  auto row = binomial_row(n);

  // Display a few notable values
  std::cout << "C(100, 0)  = " << row[0] << "\n";
  std::cout << "C(100, 1)  = " << row[1] << "\n";
  std::cout << "C(100, 10) = " << row[10] << "\n";
  std::cout << "C(100, 50) = " << row[50] << "\n";
  std::cout << "C(100,100) = " << row[100] << "\n\n";

  // Verify Pascal's identity: C(n,k) = C(n-1,k-1) + C(n-1,k)
  auto prev_row = binomial_row(n - 1);
  bool all_ok = true;
  for (int k = 1; k < n; ++k) {
    auto expected =
      prev_row[static_cast<std::size_t>(k) - 1] +
      prev_row[static_cast<std::size_t>(k)];
    if (!(row[static_cast<std::size_t>(k)] == expected)) {
      std::cout << "FAIL: Pascal's identity at k=" << k << "\n";
      all_ok = false;
    }
  }

  if (all_ok) {
    std::cout << "Pascal's identity C(100,k) = C(99,k-1) + C(99,k) "
              << "verified for all k in [1..99].\n\n";
  }

  // Verify symmetry: C(n,k) = C(n,n-k)
  bool symmetric = true;
  for (int k = 0; k <= n / 2; ++k) {
    if (!(row[static_cast<std::size_t>(k)] ==
          row[static_cast<std::size_t>(n - k)])) {
      std::cout << "FAIL: symmetry at k=" << k << "\n";
      symmetric = false;
    }
  }
  if (symmetric) {
    std::cout << "Symmetry C(100,k) = C(100,100-k) verified.\n\n";
  }

  // Show the sum of the row equals 2^100
  BigInt sum(0);
  for (auto& c : row) {
    sum += c;
  }
  BigInt two_to_100(1);
  for (int i = 0; i < n; ++i) {
    two_to_100 *= BigInt(2);
  }
  std::cout << "Sum of row  = " << sum << "\n";
  std::cout << "2^100       = " << two_to_100 << "\n";
  std::cout << "Match: " << (sum == two_to_100 ? "yes" : "no") << "\n";

  return 0;
}
