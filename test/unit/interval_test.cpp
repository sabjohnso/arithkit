#include <arithkit/concepts.hpp>
#include <arithkit/interval.hpp>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "laws/interval_arithmetic_laws.hpp"

using Catch::Approx;
using namespace arithkit;

using Ivd = Interval<double>;

// --- Static concept checks ---

static_assert(IntervalArithmetic<Ivd>);

// Note: Interval syntactically satisfies Ring (has all operators), but
// semantically it is NOT a ring — it fails distributivity (only
// subdistributivity holds).  This is tested by property tests, not
// static_assert.

// --- constexpr checks ---
static_assert(
  Interval<double>(1.0, 3.0) + Interval<double>(2.0, 4.0) ==
  Interval<double>(3.0, 7.0));
static_assert(Interval<double>().lo() == 0.0);

// --- RapidCheck generators ---

namespace rc {

  template <>
  struct Arbitrary<Ivd> {
    static Gen<Ivd> arbitrary() {
      return gen::exec([] {
        auto a = *gen::inRange(-100, 101);
        auto b = *gen::inRange(-100, 101);
        double lo = double(std::min(a, b));
        double hi = double(std::max(a, b));
        return Ivd(lo, hi);
      });
    }
  };

} // namespace rc

// --- Construction tests ---

TEST_CASE("Interval default constructs to [0,0]", "[interval]") {
  Ivd z;
  CHECK(z == Ivd(0.0, 0.0));
}

TEST_CASE("Interval from single value", "[interval]") {
  Ivd z(3.0);
  CHECK(z.lo() == 3.0);
  CHECK(z.hi() == 3.0);
}

TEST_CASE("Interval from bounds", "[interval]") {
  Ivd z(1.0, 3.0);
  CHECK(z.lo() == 1.0);
  CHECK(z.hi() == 3.0);
}

TEST_CASE("Interval swaps misordered bounds", "[interval]") {
  Ivd z(5.0, 2.0);
  CHECK(z.lo() == 2.0);
  CHECK(z.hi() == 5.0);
}

// --- Basic arithmetic ---

TEST_CASE("Interval addition", "[interval]") {
  // [1,2] + [3,4] = [4,6]
  CHECK(Ivd(1.0, 2.0) + Ivd(3.0, 4.0) == Ivd(4.0, 6.0));
}

TEST_CASE("Interval subtraction", "[interval]") {
  // [1,2] - [3,4] = [1-4, 2-3] = [-3,-1]
  CHECK(Ivd(1.0, 2.0) - Ivd(3.0, 4.0) == Ivd(-3.0, -1.0));
}

TEST_CASE("Interval multiplication", "[interval]") {
  // [2,3] * [4,5] = [8,15]
  CHECK(Ivd(2.0, 3.0) * Ivd(4.0, 5.0) == Ivd(8.0, 15.0));

  // [-1,2] * [3,4] = [-4,8]
  CHECK(Ivd(-1.0, 2.0) * Ivd(3.0, 4.0) == Ivd(-4.0, 8.0));

  // [-2,-1] * [-4,-3] = [3,8]
  CHECK(Ivd(-2.0, -1.0) * Ivd(-4.0, -3.0) == Ivd(3.0, 8.0));
}

TEST_CASE("Interval division", "[interval]") {
  // [2,6] / [2,3] = [2/3, 3]
  auto result = Ivd(2.0, 6.0) / Ivd(2.0, 3.0);
  CHECK(result.lo() == Approx(2.0 / 3.0));
  CHECK(result.hi() == Approx(3.0));
}

// --- Containment ---

TEST_CASE("Interval contains", "[interval]") {
  auto x = Ivd(1.0, 5.0);
  CHECK(x.contains(3.0));
  CHECK(x.contains(1.0));
  CHECK(x.contains(5.0));
  CHECK_FALSE(x.contains(0.0));
  CHECK_FALSE(x.contains(6.0));
}

TEST_CASE("Interval subset", "[interval]") {
  CHECK(Ivd(2.0, 3.0).is_subset_of(Ivd(1.0, 4.0)));
  CHECK(Ivd(1.0, 4.0).is_subset_of(Ivd(1.0, 4.0)));
  CHECK_FALSE(Ivd(0.0, 3.0).is_subset_of(Ivd(1.0, 4.0)));
}

// --- Negation ---

TEST_CASE("Interval negation", "[interval]") {
  CHECK(-Ivd(1.0, 3.0) == Ivd(-3.0, -1.0));
  CHECK(-Ivd(-2.0, 5.0) == Ivd(-5.0, 2.0));
}

// --- Width ---

TEST_CASE("Interval width", "[interval]") {
  CHECK(Ivd(1.0, 3.0).width() == Approx(2.0));
  CHECK(Ivd(5.0, 5.0).width() == Approx(0.0));
}

// --- Midpoint ---

TEST_CASE("Interval midpoint", "[interval]") {
  CHECK(Ivd(1.0, 3.0).midpoint() == Approx(2.0));
  CHECK(Ivd(-4.0, 2.0).midpoint() == Approx(-1.0));
}

// --- Property tests: interval arithmetic laws ---

TEST_CASE("Interval<double> satisfies interval arithmetic laws",
          "[laws][interval]") {
  arithkit::laws::check_interval_arithmetic<double>();
}
