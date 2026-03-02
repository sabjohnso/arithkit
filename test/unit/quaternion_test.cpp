#include <arithkit/bigint.hpp>
#include <arithkit/concepts.hpp>
#include <arithkit/fraction.hpp>
#include <arithkit/quaternion.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "laws/division_ring_laws.hpp"

using namespace arithkit;

using BigQ = Fraction<BigInt>;
using Qd = Quaternion<double>;
using Qq = Quaternion<BigQ>;

// --- Static concept checks ---

static_assert(DivisionRing<Qd>);
static_assert(DivisionRing<Qq>);

// Note: Quaternion syntactically satisfies Field (has all operators),
// but semantically is NOT a field — multiplication is not commutative.
// Non-commutativity is verified by the property test below.

// Not ordered
static_assert(!std::totally_ordered<Qd>);

// --- RapidCheck generators ---

namespace rc {

  template <>
  struct Arbitrary<Qq> {
    static Gen<Qq> arbitrary() {
      return gen::exec([] {
        auto gen_comp = [] {
          auto n = *gen::inRange(-20, 21);
          auto d = *gen::inRange(1, 21);
          return BigQ(BigInt(n), BigInt(d));
        };
        return Qq(gen_comp(), gen_comp(), gen_comp(), gen_comp());
      });
    }
  };

} // namespace rc

// --- Construction tests ---

TEST_CASE("Quaternion default constructs to zero", "[quaternion]") {
  Qd z;
  CHECK(z == Qd(0.0, 0.0, 0.0, 0.0));
}

TEST_CASE("Quaternion from scalar", "[quaternion]") {
  Qd z(3.0);
  CHECK(z.w() == 3.0);
  CHECK(z.x() == 0.0);
  CHECK(z.y() == 0.0);
  CHECK(z.z() == 0.0);
}

TEST_CASE("Quaternion from four components", "[quaternion]") {
  Qd z(1.0, 2.0, 3.0, 4.0);
  CHECK(z.w() == 1.0);
  CHECK(z.x() == 2.0);
  CHECK(z.y() == 3.0);
  CHECK(z.z() == 4.0);
}

// --- Basic arithmetic ---

TEST_CASE("Quaternion addition", "[quaternion]") {
  auto a = Qd(1.0, 2.0, 3.0, 4.0);
  auto b = Qd(5.0, 6.0, 7.0, 8.0);
  CHECK(a + b == Qd(6.0, 8.0, 10.0, 12.0));
}

TEST_CASE("Quaternion subtraction", "[quaternion]") {
  auto a = Qd(5.0, 6.0, 7.0, 8.0);
  auto b = Qd(1.0, 2.0, 3.0, 4.0);
  CHECK(a - b == Qd(4.0, 4.0, 4.0, 4.0));
}

TEST_CASE("Quaternion multiplication (Hamilton rules)", "[quaternion]") {
  // i*j = k
  CHECK(Qd(0, 1, 0, 0) * Qd(0, 0, 1, 0) == Qd(0, 0, 0, 1));
  // j*i = -k
  CHECK(Qd(0, 0, 1, 0) * Qd(0, 1, 0, 0) == Qd(0, 0, 0, -1));
  // j*k = i
  CHECK(Qd(0, 0, 1, 0) * Qd(0, 0, 0, 1) == Qd(0, 1, 0, 0));
  // k*j = -i
  CHECK(Qd(0, 0, 0, 1) * Qd(0, 0, 1, 0) == Qd(0, -1, 0, 0));
  // k*i = j
  CHECK(Qd(0, 0, 0, 1) * Qd(0, 1, 0, 0) == Qd(0, 0, 1, 0));
  // i*k = -j
  CHECK(Qd(0, 1, 0, 0) * Qd(0, 0, 0, 1) == Qd(0, 0, -1, 0));
  // i*i = -1
  CHECK(Qd(0, 1, 0, 0) * Qd(0, 1, 0, 0) == Qd(-1, 0, 0, 0));
  // j*j = -1
  CHECK(Qd(0, 0, 1, 0) * Qd(0, 0, 1, 0) == Qd(-1, 0, 0, 0));
  // k*k = -1
  CHECK(Qd(0, 0, 0, 1) * Qd(0, 0, 0, 1) == Qd(-1, 0, 0, 0));
}

TEST_CASE("Quaternion multiplication is non-commutative",
          "[quaternion]") {
  auto a = Qd(1.0, 2.0, 3.0, 4.0);
  auto b = Qd(5.0, 6.0, 7.0, 8.0);
  CHECK_FALSE(a * b == b * a);
}

// --- Conjugate and norm ---

TEST_CASE("Quaternion conjugate", "[quaternion]") {
  auto q = Qd(1.0, 2.0, 3.0, 4.0);
  CHECK(conjugate(q) == Qd(1.0, -2.0, -3.0, -4.0));
}

TEST_CASE("Quaternion norm", "[quaternion]") {
  // norm(1+2i+3j+4k) = 1+4+9+16 = 30
  auto q = Qq(BigQ(BigInt(1)), BigQ(BigInt(2)), BigQ(BigInt(3)),
              BigQ(BigInt(4)));
  CHECK(norm(q) == BigQ(BigInt(30)));
}

// --- Negation ---

TEST_CASE("Quaternion negation", "[quaternion]") {
  auto q = Qd(1.0, -2.0, 3.0, -4.0);
  CHECK(-q == Qd(-1.0, 2.0, -3.0, 4.0));
}

// --- Division (exact) ---

TEST_CASE("Quaternion division by itself", "[quaternion]") {
  auto one = BigQ(BigInt(1));
  auto q = Qq(BigQ(BigInt(1)), BigQ(BigInt(2)), BigQ(BigInt(3)),
              BigQ(BigInt(4)));
  auto result = q / q;
  CHECK(result == Qq(one));
}

// --- Algebraic law suites ---

TEST_CASE("Quaternion<Fraction<BigInt>> satisfies DivisionRing laws",
          "[laws][quaternion]") {
  arithkit::laws::check_division_ring<Qq>();
}
