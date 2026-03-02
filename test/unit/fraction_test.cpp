#include <arithkit/bigint.hpp>
#include <arithkit/concepts.hpp>
#include <arithkit/fraction.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "laws/field_laws.hpp"
#include "laws/ordered_field_laws.hpp"

using namespace arithkit;

using Q = Fraction<int>;
using BigQ = Fraction<BigInt>;

// --- Static concept checks ---

static_assert(Field<Q>);
static_assert(OrderedField<Q>);
static_assert(Field<BigQ>);

// --- RapidCheck generators ---

namespace rc {

  template <>
  struct Arbitrary<Q> {
    static Gen<Q> arbitrary() {
      return gen::exec([] {
        auto n = *gen::inRange(-100, 101);
        auto d = *gen::inRange(1, 101);
        return Q(n, d);
      });
    }
  };

  template <>
  struct Arbitrary<BigQ> {
    static Gen<BigQ> arbitrary() {
      return gen::exec([] {
        auto n = *gen::inRange(-100, 101);
        auto d = *gen::inRange(1, 101);
        return BigQ(BigInt(n), BigInt(d));
      });
    }
  };

} // namespace rc

// --- Construction tests ---

TEST_CASE("Fraction default constructs to 0/1", "[fraction]") {
  Q f;
  CHECK(f == Q(0, 1));
}

TEST_CASE("Fraction reduces to lowest terms", "[fraction]") {
  CHECK(Q(2, 4) == Q(1, 2));
  CHECK(Q(6, 9) == Q(2, 3));
  CHECK(Q(-4, 6) == Q(-2, 3));
  CHECK(Q(4, -6) == Q(-2, 3));
  CHECK(Q(-4, -6) == Q(2, 3));
}

TEST_CASE("Fraction from integer", "[fraction]") {
  CHECK(Q(5) == Q(5, 1));
  CHECK(Q(0) == Q(0, 1));
  CHECK(Q(-3) == Q(-3, 1));
}

// --- Algebraic law suites ---

TEST_CASE("Fraction<int> satisfies Field laws", "[laws][fraction]") {
  arithkit::laws::check_field<Q>();
}

TEST_CASE("Fraction<int> satisfies OrderedField laws",
          "[laws][fraction]") {
  arithkit::laws::check_ordered_field<Q>();
}

TEST_CASE("Fraction<BigInt> satisfies Field laws",
          "[laws][fraction][bigint]") {
  arithkit::laws::check_field<BigQ>();
}

// --- Arithmetic edge cases ---

TEST_CASE("Fraction division", "[fraction]") {
  CHECK(Q(1, 2) / Q(1, 3) == Q(3, 2));
  CHECK(Q(2, 3) / Q(4, 5) == Q(5, 6));
}

TEST_CASE("Fraction negation", "[fraction]") {
  CHECK(-Q(1, 2) == Q(-1, 2));
  CHECK(-Q(-3, 4) == Q(3, 4));
  CHECK(-Q(0, 1) == Q(0, 1));
}

// --- String conversion ---

TEST_CASE("Fraction to_string", "[fraction]") {
  CHECK(to_string(Q(1, 2)) == "1/2");
  CHECK(to_string(Q(3, 1)) == "3");
  CHECK(to_string(Q(0, 1)) == "0");
  CHECK(to_string(Q(-5, 7)) == "-5/7");
}
