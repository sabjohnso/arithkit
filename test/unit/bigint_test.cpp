#include <arithkit/bigint.hpp>
#include <arithkit/concepts.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "laws/euclidean_domain_laws.hpp"

using namespace arithkit;

// --- Static concept checks ---

static_assert(EuclideanDomain<BigInt>);

// --- RapidCheck generator for BigInt ---

namespace rc {

  template <>
  struct Arbitrary<BigInt> {
    static Gen<BigInt> arbitrary() {
      return gen::map(gen::inRange(-100000, 100001),
                      [](int n) { return BigInt(n); });
    }
  };

} // namespace rc

// --- Construction and basic operations ---

TEST_CASE("BigInt default constructs to zero", "[bigint]") {
  BigInt z;
  CHECK(z == BigInt(0));
}

TEST_CASE("BigInt constructs from int", "[bigint]") {
  CHECK(BigInt(42) == BigInt(42));
  CHECK(BigInt(-7) == BigInt(-7));
  CHECK(BigInt(0) == BigInt(0));
}

TEST_CASE("BigInt string representation", "[bigint]") {
  CHECK(to_string(BigInt(0)) == "0");
  CHECK(to_string(BigInt(42)) == "42");
  CHECK(to_string(BigInt(-42)) == "-42");
  CHECK(to_string(BigInt(1000000)) == "1000000");
}

// --- Algebraic law suites ---

TEST_CASE("BigInt satisfies EuclideanDomain laws", "[laws][bigint]") {
  arithkit::laws::check_euclidean_domain<BigInt>();
}

// --- GCD tests ---

TEST_CASE("BigInt gcd", "[bigint]") {
  CHECK(gcd(BigInt(12), BigInt(8)) == BigInt(4));
  CHECK(gcd(BigInt(0), BigInt(5)) == BigInt(5));
  CHECK(gcd(BigInt(7), BigInt(0)) == BigInt(7));
  CHECK(gcd(BigInt(0), BigInt(0)) == BigInt(0));
  CHECK(gcd(BigInt(-12), BigInt(8)) == BigInt(4));
  CHECK(gcd(BigInt(17), BigInt(13)) == BigInt(1));
}

TEST_CASE("BigInt gcd property: gcd(a,b) divides both", "[bigint]") {
  rc::prop("gcd(a,b) divides a and b", [](BigInt a, BigInt b) {
    auto g = gcd(a, b);
    if (g != BigInt(0)) {
      RC_ASSERT(a % g == BigInt(0));
      RC_ASSERT(b % g == BigInt(0));
    }
  });
}

// --- Large value tests ---

TEST_CASE("BigInt handles values beyond int range", "[bigint]") {
  auto large = BigInt(1000000000LL) * BigInt(1000000000LL);
  CHECK(to_string(large) == "1000000000000000000");
}
