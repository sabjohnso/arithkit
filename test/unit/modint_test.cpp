#include <arithkit/concepts.hpp>
#include <arithkit/modint.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "laws/commutative_ring_laws.hpp"
#include "laws/field_laws.hpp"

using namespace arithkit;

using M6 = ModInt<6>;
using M7 = ModInt<7>;
using M2 = ModInt<2>;

// --- Static concept checks ---

static_assert(CommutativeRing<M6>);
static_assert(CommutativeRing<M7>);
static_assert(CommutativeRing<M2>);

// M7 is a field (7 is prime), M6 is not (6 = 2*3)
static_assert(Field<M7>);
static_assert(!Field<M6>);
static_assert(Field<M2>);

// --- constexpr checks ---
static_assert(ModInt<7>(3) + ModInt<7>(5) == ModInt<7>(1));
static_assert(ModInt<7>(3) * ModInt<7>(4) == ModInt<7>(5));
static_assert(-ModInt<7>(3) == ModInt<7>(4));

// --- RapidCheck generators ---

namespace rc {

  template <>
  struct Arbitrary<M6> {
    static Gen<M6> arbitrary() {
      return gen::exec(
        [] { return M6(*gen::inRange(0, 6)); });
    }
  };

  template <>
  struct Arbitrary<M7> {
    static Gen<M7> arbitrary() {
      return gen::exec(
        [] { return M7(*gen::inRange(0, 7)); });
    }
  };

  template <>
  struct Arbitrary<M2> {
    static Gen<M2> arbitrary() {
      return gen::exec(
        [] { return M2(*gen::inRange(0, 2)); });
    }
  };

} // namespace rc

// --- Construction tests ---

TEST_CASE("ModInt default constructs to zero", "[modint]") {
  M7 z;
  CHECK(z == M7(0));
}

TEST_CASE("ModInt from value reduces modulo N", "[modint]") {
  CHECK(M7(10) == M7(3));
  CHECK(M7(-1) == M7(6));
  CHECK(M7(7) == M7(0));
}

TEST_CASE("ModInt value accessor", "[modint]") {
  CHECK(M7(5).value() == 5);
  CHECK(M6(4).value() == 4);
}

// --- Basic arithmetic ---

TEST_CASE("ModInt addition", "[modint]") {
  CHECK(M7(3) + M7(5) == M7(1));
  CHECK(M6(4) + M6(3) == M6(1));
}

TEST_CASE("ModInt subtraction", "[modint]") {
  CHECK(M7(2) - M7(5) == M7(4));
  CHECK(M6(1) - M6(4) == M6(3));
}

TEST_CASE("ModInt multiplication", "[modint]") {
  CHECK(M7(3) * M7(4) == M7(5));
  CHECK(M6(3) * M6(4) == M6(0));
}

TEST_CASE("ModInt negation", "[modint]") {
  CHECK(-M7(0) == M7(0));
  CHECK(-M7(3) == M7(4));
  CHECK(-M6(5) == M6(1));
}

// --- Division (prime moduli only) ---

TEST_CASE("ModInt<7> division", "[modint]") {
  // 3 * 5 = 15 = 1 (mod 7), so 1/3 = 5
  CHECK(M7(1) / M7(3) == M7(5));
  // a / a == 1
  CHECK(M7(4) / M7(4) == M7(1));
}

// --- Mixed int operators ---

TEST_CASE("ModInt mixed int operators", "[modint]") {
  CHECK(M7(3) + 5 == M7(1));
  CHECK(2 + M7(6) == M7(1));
  CHECK(M7(3) * 4 == M7(5));
  CHECK(4 * M7(3) == M7(5));
  CHECK(M7(5) - 2 == M7(3));
  CHECK(10 - M7(3) == M7(0));
}

// --- Algebraic law suites ---

TEST_CASE("ModInt<6> satisfies CommutativeRing laws",
          "[laws][modint]") {
  arithkit::laws::check_commutative_ring<M6>();
}

TEST_CASE("ModInt<7> satisfies CommutativeRing laws",
          "[laws][modint]") {
  arithkit::laws::check_commutative_ring<M7>();
}

TEST_CASE("ModInt<7> satisfies Field laws", "[laws][modint]") {
  arithkit::laws::check_field<M7>();
}

TEST_CASE("ModInt<2> field division", "[modint]") {
  // Z/2Z has only {0, 1}; the only non-zero element is 1.
  CHECK(M2(1) / M2(1) == M2(1));
}
