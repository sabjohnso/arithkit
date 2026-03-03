#include <arithkit/concepts.hpp>
#include <arithkit/simd.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "laws/commutative_ring_laws.hpp"

using namespace arithkit;

using S4d = Simd<double, 4>;
using S2i = Simd<int, 2>;

// --- Static concept checks ---

// Simd<T,N> is a product ring — syntactically an OrderedField when T is,
// but semantically has zero divisors (e.g. (1,0,...)*(0,1,...) = (0,0,...)),
// so ordered field laws don't hold.  CommutativeRing laws do hold.
static_assert(CommutativeRing<S4d>);
static_assert(CommutativeRing<S2i>);

// --- constexpr checks ---
static_assert(
  Simd<int, 4>(1, 2, 3, 4) + Simd<int, 4>(5, 6, 7, 8) ==
  Simd<int, 4>(6, 8, 10, 12));
static_assert(Simd<int, 2>(3, 3) == Simd<int, 2>(3, 3));

// --- RapidCheck generators ---

namespace rc {

  template <>
  struct Arbitrary<S4d> {
    static Gen<S4d> arbitrary() {
      return gen::exec([] {
        std::array<double, 4> c;
        for (auto& v : c)
          v = double(*gen::inRange(-50, 51));
        return S4d(c);
      });
    }
  };

  template <>
  struct Arbitrary<S2i> {
    static Gen<S2i> arbitrary() {
      return gen::exec([] {
        std::array<int, 2> c;
        for (auto& v : c)
          v = *gen::inRange(-100, 101);
        return S2i(c);
      });
    }
  };

} // namespace rc

// --- Construction tests ---

TEST_CASE("Simd default constructs to zero", "[simd]") {
  S4d z;
  for (std::size_t i = 0; i < 4; ++i) {
    CHECK(z[i] == 0.0);
  }
}

TEST_CASE("Simd from scalar broadcasts", "[simd]") {
  S4d z(3.0);
  for (std::size_t i = 0; i < 4; ++i) {
    CHECK(z[i] == 3.0);
  }
}

TEST_CASE("Simd from array", "[simd]") {
  S4d z(std::array<double, 4>{1.0, 2.0, 3.0, 4.0});
  CHECK(z[0] == 1.0);
  CHECK(z[1] == 2.0);
  CHECK(z[2] == 3.0);
  CHECK(z[3] == 4.0);
}

TEST_CASE("Simd from N individual values (S4d)", "[simd]") {
  S4d z(1.0, 2.0, 3.0, 4.0);
  CHECK(z[0] == 1.0);
  CHECK(z[1] == 2.0);
  CHECK(z[2] == 3.0);
  CHECK(z[3] == 4.0);
}

TEST_CASE("Simd from N individual values (S2i)", "[simd]") {
  S2i z(10, 20);
  CHECK(z[0] == 10);
  CHECK(z[1] == 20);
}

TEST_CASE("Simd scalar broadcast unaffected by N-value constructor", "[simd]") {
  S4d b(7.0);
  for (std::size_t i = 0; i < 4; ++i) {
    CHECK(b[i] == 7.0);
  }
}

// --- Basic arithmetic ---

TEST_CASE("Simd addition", "[simd]") {
  auto a = S4d(std::array{1.0, 2.0, 3.0, 4.0});
  auto b = S4d(std::array{5.0, 6.0, 7.0, 8.0});
  auto c = a + b;
  CHECK(c == S4d(std::array{6.0, 8.0, 10.0, 12.0}));
}

TEST_CASE("Simd multiplication", "[simd]") {
  auto a = S4d(std::array{1.0, 2.0, 3.0, 4.0});
  auto b = S4d(std::array{5.0, 6.0, 7.0, 8.0});
  auto c = a * b;
  CHECK(c == S4d(std::array{5.0, 12.0, 21.0, 32.0}));
}

TEST_CASE("Simd division", "[simd]") {
  auto a = S4d(std::array{10.0, 20.0, 30.0, 40.0});
  auto b = S4d(std::array{2.0, 4.0, 5.0, 8.0});
  auto c = a / b;
  CHECK(c == S4d(std::array{5.0, 5.0, 6.0, 5.0}));
}

// --- Negation ---

TEST_CASE("Simd negation", "[simd]") {
  auto a = S4d(std::array{1.0, -2.0, 3.0, -4.0});
  CHECK(-a == S4d(std::array{-1.0, 2.0, -3.0, 4.0}));
}

// --- Algebraic law suites ---

TEST_CASE("Simd<double,4> satisfies CommutativeRing laws",
          "[laws][simd]") {
  arithkit::laws::check_commutative_ring<S4d>();
}

TEST_CASE("Simd<int,2> satisfies CommutativeRing laws",
          "[laws][simd]") {
  arithkit::laws::check_commutative_ring<S2i>();
}
