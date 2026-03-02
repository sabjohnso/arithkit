#include <arithkit/bigint.hpp>
#include <arithkit/complex.hpp>
#include <arithkit/concepts.hpp>
#include <arithkit/fraction.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "laws/commutative_ring_laws.hpp"
#include "laws/complex_field_extension_laws.hpp"
#include "laws/complex_ring_extension_laws.hpp"
#include "laws/field_laws.hpp"

using namespace arithkit;

using Cd = Complex<double>;
using Ci = Complex<int>;
using Cq = Complex<Fraction<int>>;
using BigQ = Fraction<BigInt>;
using CbigQ = Complex<BigQ>;

// --- Static concept checks ---

static_assert(Ring<Ci>);
static_assert(Field<Cd>);
static_assert(Field<Cq>);
static_assert(Field<CbigQ>);

// Complex is NOT ordered (no total order on complex numbers)
static_assert(!std::totally_ordered<Cd>);

// --- RapidCheck generators ---

namespace rc {

  template <>
  struct Arbitrary<Cd> {
    static Gen<Cd> arbitrary() {
      return gen::exec([] {
        auto r = *gen::inRange(-100, 101);
        auto i = *gen::inRange(-100, 101);
        return Cd(double(r), double(i));
      });
    }
  };

  template <>
  struct Arbitrary<Ci> {
    static Gen<Ci> arbitrary() {
      return gen::exec([] {
        auto r = *gen::inRange(-100, 101);
        auto i = *gen::inRange(-100, 101);
        return Ci(r, i);
      });
    }
  };

  template <>
  struct Arbitrary<CbigQ> {
    static Gen<CbigQ> arbitrary() {
      return gen::exec([] {
        auto n1 = *gen::inRange(-50, 51);
        auto d1 = *gen::inRange(1, 51);
        auto n2 = *gen::inRange(-50, 51);
        auto d2 = *gen::inRange(1, 51);
        return CbigQ(BigQ(BigInt(n1), BigInt(d1)),
                      BigQ(BigInt(n2), BigInt(d2)));
      });
    }
  };

} // namespace rc

// --- Construction tests ---

TEST_CASE("Complex default constructs to zero", "[complex]") {
  Cd z;
  CHECK(z == Cd(0.0, 0.0));
}

TEST_CASE("Complex from single value", "[complex]") {
  Cd z(3.0);
  CHECK(z.real() == 3.0);
  CHECK(z.imag() == 0.0);
}

TEST_CASE("Complex from two values", "[complex]") {
  Cd z(1.0, 2.0);
  CHECK(z.real() == 1.0);
  CHECK(z.imag() == 2.0);
}

// --- Basic arithmetic ---

TEST_CASE("Complex addition", "[complex]") {
  auto a = Cd(1.0, 2.0);
  auto b = Cd(3.0, 4.0);
  CHECK(a + b == Cd(4.0, 6.0));
}

TEST_CASE("Complex subtraction", "[complex]") {
  auto a = Cd(5.0, 3.0);
  auto b = Cd(2.0, 1.0);
  CHECK(a - b == Cd(3.0, 2.0));
}

TEST_CASE("Complex multiplication", "[complex]") {
  // (1+2i)(3+4i) = 3+4i+6i+8i² = 3+10i-8 = -5+10i
  auto a = Cd(1.0, 2.0);
  auto b = Cd(3.0, 4.0);
  CHECK(a * b == Cd(-5.0, 10.0));
}

TEST_CASE("Complex division", "[complex]") {
  // (1+0i) / (1+0i) = 1
  CHECK(Cd(1.0) / Cd(1.0) == Cd(1.0));

  // Using exact fractions: (1+2i)/(3+4i) = (11+2i)/25
  using Q = Fraction<int>;
  auto a = Cq(Q(1), Q(2));
  auto b = Cq(Q(3), Q(4));
  auto result = a / b;
  CHECK(result == Cq(Q(11, 25), Q(2, 25)));
}

// --- Conjugate and norm ---

TEST_CASE("Complex conjugate", "[complex]") {
  auto z = Cd(3.0, 4.0);
  CHECK(conjugate(z) == Cd(3.0, -4.0));
}

TEST_CASE("Complex norm", "[complex]") {
  // norm(3+4i) = 9+16 = 25
  using Q = Fraction<int>;
  auto z = Cq(Q(3), Q(4));
  CHECK(norm(z) == Q(25));
}

// --- Negation ---

TEST_CASE("Complex negation", "[complex]") {
  auto z = Cd(1.0, -2.0);
  CHECK(-z == Cd(-1.0, 2.0));
}

// --- Algebraic law suites ---

TEST_CASE("Complex<Fraction<BigInt>> satisfies Field laws",
          "[laws][complex]") {
  arithkit::laws::check_field<CbigQ>();
}

TEST_CASE("Complex<int> satisfies CommutativeRing laws",
          "[laws][complex]") {
  arithkit::laws::check_commutative_ring<Ci>();
}

TEST_CASE("Complex<int> satisfies ring extension laws", "[laws][complex]") {
  arithkit::laws::check_complex_ring_extension<int>();
}

TEST_CASE(
  "Complex<Fraction<BigInt>> satisfies field extension laws",
  "[laws][complex]") {
  arithkit::laws::check_complex_field_extension<BigQ>();
}
