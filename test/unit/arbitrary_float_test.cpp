#include <arithkit/arbitrary_float.hpp>
#include <arithkit/concepts.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "laws/approx_ordered_field_laws.hpp"

using namespace arithkit;

// --- Static concept checks ---

static_assert(OrderedField<ArbitraryFloat>);

// --- RapidCheck generator ---

namespace rc {

  template <>
  struct Arbitrary<ArbitraryFloat> {
    static Gen<ArbitraryFloat> arbitrary() {
      return gen::exec([] {
        auto n = *gen::inRange(-10000, 10001);
        auto e = *gen::inRange(-5, 6);
        return ArbitraryFloat(n, e);
      });
    }
  };

} // namespace rc

// --- Construction tests ---

TEST_CASE("ArbitraryFloat default constructs to zero",
          "[arbitrary_float]") {
  ArbitraryFloat z;
  CHECK(z == ArbitraryFloat(0));
}

TEST_CASE("ArbitraryFloat from integer", "[arbitrary_float]") {
  CHECK(ArbitraryFloat(42) == ArbitraryFloat(42));
  CHECK(ArbitraryFloat(-7) == ArbitraryFloat(-7));
}

TEST_CASE("ArbitraryFloat basic arithmetic", "[arbitrary_float]") {
  auto a = ArbitraryFloat(1, -1); // 0.1 in base 10
  auto b = ArbitraryFloat(2, -1); // 0.2 in base 10
  auto c = a + b;
  CHECK(c == ArbitraryFloat(3, -1));
}

// --- Approximate law suite ---

TEST_CASE("ArbitraryFloat satisfies OrderedField laws (approximate)",
          "[laws][arbitrary_float]") {
  arithkit::laws::check_approx_ordered_field<ArbitraryFloat>();
}
