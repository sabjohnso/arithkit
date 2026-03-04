#include <arithkit/concepts.hpp>
#include <arithkit/fixed_point.hpp>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

// Provide a FixedPoint-specific approx_equal in namespace arithkit so
// that ADL finds it during template instantiation of the law suite.
// Q16.16 has ~4.8 decimal digits of precision; the generic
// non-floating-point approx_equal demands ~4 digits (0.01%), which is
// too tight after division rounding.  We convert to double and use a
// tolerance of 0.1%.
#include "laws/approx_equal.hpp"

namespace arithkit {

  template <unsigned I, unsigned F>
  bool
  approx_equal(const FixedPoint<I, F>& a,
               const FixedPoint<I, F>& b) {
    return laws::approx_equal(a.to_double(), b.to_double(), 1e-2,
                              1e-6);
  }

} // namespace arithkit

#include "laws/approx_ordered_field_laws.hpp"

using namespace arithkit;

using FP = FixedPoint<16, 16>;
using FP8 = FixedPoint<24, 8>;

// --- Static concept checks ---

static_assert(OrderedField<FP>);
static_assert(std::totally_ordered<FP>);
static_assert(OrderedField<FP8>);

// --- RapidCheck generators ---

namespace rc {

  template <>
  struct Arbitrary<FP> {
    static Gen<FP> arbitrary() {
      return gen::exec([] {
        // Q16.16 has ~4.8 decimal digits of precision.  Keep values
        // in [-100, 100] so division rounding stays within 0.1%.
        auto v = *gen::inRange(-100, 101);
        auto frac = *gen::inRange(0, 100);
        return FP(double(v) + double(frac) / 100.0);
      });
    }
  };

  template <>
  struct Arbitrary<FP8> {
    static Gen<FP8> arbitrary() {
      return gen::exec([] {
        auto v = *gen::inRange(-100, 101);
        auto frac = *gen::inRange(0, 100);
        return FP8(double(v) + double(frac) / 100.0);
      });
    }
  };

} // namespace rc

// --- Construction tests ---

TEST_CASE("FixedPoint default constructs to zero", "[fixed_point]") {
  FP z;
  CHECK(z == FP(0.0));
  CHECK(z.to_double() == 0.0);
}

TEST_CASE("FixedPoint from double", "[fixed_point]") {
  FP a(1.5);
  CHECK(a.to_double() == Catch::Approx(1.5).epsilon(1e-4));
}

TEST_CASE("FixedPoint from integer value", "[fixed_point]") {
  FP a(42.0);
  CHECK(a.to_double() == Catch::Approx(42.0).epsilon(1e-4));
}

// --- Basic arithmetic ---

TEST_CASE("FixedPoint addition", "[fixed_point]") {
  auto a = FP(1.25);
  auto b = FP(2.75);
  CHECK((a + b).to_double() == Catch::Approx(4.0).epsilon(1e-4));
}

TEST_CASE("FixedPoint subtraction", "[fixed_point]") {
  auto a = FP(5.5);
  auto b = FP(2.25);
  CHECK((a - b).to_double() == Catch::Approx(3.25).epsilon(1e-4));
}

TEST_CASE("FixedPoint multiplication", "[fixed_point]") {
  auto a = FP(3.0);
  auto b = FP(2.5);
  CHECK((a * b).to_double() == Catch::Approx(7.5).epsilon(1e-4));
}

TEST_CASE("FixedPoint division", "[fixed_point]") {
  auto a = FP(7.5);
  auto b = FP(2.5);
  CHECK((a / b).to_double() == Catch::Approx(3.0).epsilon(1e-4));
}

TEST_CASE("FixedPoint negation", "[fixed_point]") {
  auto a = FP(3.14);
  CHECK((-a).to_double() == Catch::Approx(-3.14).epsilon(1e-3));
}

// --- Ordering ---

TEST_CASE("FixedPoint ordering", "[fixed_point]") {
  CHECK(FP(1.0) < FP(2.0));
  CHECK(FP(2.0) > FP(1.0));
  CHECK(FP(1.0) == FP(1.0));
  CHECK(FP(-1.0) < FP(0.0));
}

// --- Algebraic law suites ---

TEST_CASE("FixedPoint<16,16> satisfies approx OrderedField laws",
          "[laws][fixed_point]") {
  arithkit::laws::check_approx_ordered_field<FP>();
}
