#include <arithkit/concepts.hpp>
#include <arithkit/fraction.hpp>
#include <arithkit/jet.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "laws/commutative_ring_laws.hpp"

using namespace arithkit;

using J3d = Jet<double, 3>;
using J3i = Jet<int, 3>;
using J3q = Jet<Fraction<int>, 3>;

// --- Static concept checks ---

static_assert(CommutativeRing<J3d>);
static_assert(CommutativeRing<J3i>);
static_assert(CommutativeRing<J3q>);

// Jet is NOT a field — higher-order nilpotents are zero divisors
static_assert(!Field<J3d>);

// --- constexpr checks ---
static_assert(
  Jet<int, 2>(std::array<int, 3>{1, 2, 3}) +
      Jet<int, 2>(std::array<int, 3>{4, 5, 6}) ==
  Jet<int, 2>(std::array<int, 3>{5, 7, 9}));

// --- RapidCheck generators ---

namespace rc {

  template <>
  struct Arbitrary<J3i> {
    static Gen<J3i> arbitrary() {
      return gen::exec([] {
        std::array<int, 4> c;
        for (auto& v : c) v = *gen::inRange(-50, 51);
        return J3i(c);
      });
    }
  };

  template <>
  struct Arbitrary<J3d> {
    static Gen<J3d> arbitrary() {
      return gen::exec([] {
        std::array<double, 4> c;
        for (auto& v : c) v = double(*gen::inRange(-50, 51));
        return J3d(c);
      });
    }
  };

  template <>
  struct Arbitrary<J3q> {
    static Gen<J3q> arbitrary() {
      return gen::exec([] {
        using Q = Fraction<int>;
        std::array<Q, 4> c;
        for (auto& v : c) {
          auto n = *gen::inRange(-3, 4);
          auto d = *gen::inRange(1, 4);
          v = Q(n, d);
        }
        return J3q(c);
      });
    }
  };

} // namespace rc

// --- Construction tests ---

TEST_CASE("Jet default constructs to zero", "[jet]") {
  J3d z;
  CHECK(z == J3d());
  CHECK(z.value() == 0.0);
  for (std::size_t i = 0; i <= 3; ++i) CHECK(z[i] == 0.0);
}

TEST_CASE("Jet from scalar", "[jet]") {
  J3d z(5.0);
  CHECK(z.value() == 5.0);
  CHECK(z[0] == 5.0);
  CHECK(z[1] == 0.0);
  CHECK(z[2] == 0.0);
  CHECK(z[3] == 0.0);
}

TEST_CASE("Jet from array", "[jet]") {
  auto z = J3d(std::array<double, 4>{1.0, 2.0, 3.0, 4.0});
  CHECK(z[0] == 1.0);
  CHECK(z[1] == 2.0);
  CHECK(z[2] == 3.0);
  CHECK(z[3] == 4.0);
}

// --- Basic arithmetic ---

TEST_CASE("Jet addition", "[jet]") {
  auto a = J3i(std::array<int, 4>{1, 2, 3, 4});
  auto b = J3i(std::array<int, 4>{5, 6, 7, 8});
  auto c = a + b;
  CHECK(c == J3i(std::array<int, 4>{6, 8, 10, 12}));
}

TEST_CASE("Jet subtraction", "[jet]") {
  auto a = J3i(std::array<int, 4>{5, 6, 7, 8});
  auto b = J3i(std::array<int, 4>{1, 2, 3, 4});
  auto c = a - b;
  CHECK(c == J3i(std::array<int, 4>{4, 4, 4, 4}));
}

TEST_CASE("Jet multiplication (truncated Cauchy product)", "[jet]") {
  // (1 + 2x) * (3 + 4x) = 3 + 10x + 8x² truncated to degree 3
  auto a = Jet<int, 3>(std::array<int, 4>{1, 2, 0, 0});
  auto b = Jet<int, 3>(std::array<int, 4>{3, 4, 0, 0});
  auto c = a * b;
  CHECK(c[0] == 3);
  CHECK(c[1] == 10);
  CHECK(c[2] == 8);
  CHECK(c[3] == 0);
}

TEST_CASE("Jet negation", "[jet]") {
  auto z = J3i(std::array<int, 4>{1, -2, 3, -4});
  CHECK(-z == J3i(std::array<int, 4>{-1, 2, -3, 4}));
}

// --- Automatic differentiation property ---

TEST_CASE("Jet<double,2> computes f(x)=x^3 derivatives", "[jet]") {
  // Jet seed: x = (a, 1, 0, 0) means f(a), f'(a)/1!, f''(a)/2!
  // f(x) = x^3: f(2)=8, f'(2)=12, f''(2)/2=6, f'''(2)/6=1 (but N=2)
  using J = Jet<double, 2>;
  auto x = J(std::array<double, 3>{2.0, 1.0, 0.0});
  auto r = x * x * x;
  CHECK(r[0] == 8.0);   // f(2) = 8
  CHECK(r[1] == 12.0);  // f'(2) = 12
  CHECK(r[2] == 6.0);   // f''(2)/2! = 6
}

// --- Mixed scalar operators ---

TEST_CASE("Jet mixed scalar operators", "[jet]") {
  auto z = J3i(std::array<int, 4>{1, 2, 3, 4});
  CHECK((z + 10) == J3i(std::array<int, 4>{11, 2, 3, 4}));
  CHECK((10 + z) == J3i(std::array<int, 4>{11, 2, 3, 4}));
  CHECK((z - 1) == J3i(std::array<int, 4>{0, 2, 3, 4}));
  CHECK((10 - z) == J3i(std::array<int, 4>{9, -2, -3, -4}));
  CHECK((z * 2) == J3i(std::array<int, 4>{2, 4, 6, 8}));
  CHECK((2 * z) == J3i(std::array<int, 4>{2, 4, 6, 8}));
}

// --- Algebraic law suites ---

TEST_CASE("Jet<int,3> satisfies CommutativeRing laws",
          "[laws][jet]") {
  arithkit::laws::check_commutative_ring<J3i>();
}

TEST_CASE("Jet<double,3> satisfies CommutativeRing laws",
          "[laws][jet]") {
  arithkit::laws::check_commutative_ring<J3d>();
}

TEST_CASE("Jet<Fraction<int>,3> satisfies CommutativeRing laws",
          "[laws][jet]") {
  arithkit::laws::check_commutative_ring<J3q>();
}
