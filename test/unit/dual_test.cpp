#include <arithkit/concepts.hpp>
#include <arithkit/concepts/linear_augmentation.hpp>
#include <arithkit/dual.hpp>
#include <arithkit/fraction.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "laws/commutative_ring_laws.hpp"
#include "laws/linear_augmentation_laws.hpp"

using namespace arithkit;

using Dd = Dual<double>;
using Di = Dual<int>;
using Dq = Dual<Fraction<int>>;

// --- Static concept checks ---

static_assert(CommutativeRing<Dd>);
static_assert(CommutativeRing<Di>);
static_assert(CommutativeRing<Dq>);

// Dual is NOT a field — ε is a zero divisor (ε² = 0)
static_assert(!Field<Dd>);

static_assert(LinearAugmentation<int, Dual<int>>);
static_assert(LinearAugmentation<double, Dual<double>>);
static_assert(LinearAugmentation<Fraction<int>, Dual<Fraction<int>>>);
static_assert(LinearAugmentationField<double, Dual<double>>);
static_assert(LinearAugmentationField<Fraction<int>, Dual<Fraction<int>>>);
// Dual<T> has no operator/, so Dual<T> is not a Field and cannot
// serve as the base scalar type of a LinearAugmentationField.
static_assert(!LinearAugmentationField<Dual<double>, Dual<Dual<double>>>);

// --- constexpr checks ---
static_assert(Dual<int>(3, 4) + Dual<int>(1, 2) == Dual<int>(4, 6));
static_assert(Dual<int>(3, 4) * Dual<int>(1, 2) == Dual<int>(3, 10));

// --- RapidCheck generators ---

namespace rc {

  template <>
  struct Arbitrary<Dd> {
    static Gen<Dd> arbitrary() {
      return gen::exec([] {
        auto r = *gen::inRange(-100, 101);
        auto e = *gen::inRange(-100, 101);
        return Dd(double(r), double(e));
      });
    }
  };

  template <>
  struct Arbitrary<Di> {
    static Gen<Di> arbitrary() {
      return gen::exec([] {
        auto r = *gen::inRange(-100, 101);
        auto e = *gen::inRange(-100, 101);
        return Di(r, e);
      });
    }
  };

  template <>
  struct Arbitrary<Dq> {
    static Gen<Dq> arbitrary() {
      return gen::exec([] {
        using Q = Fraction<int>;
        auto n1 = *gen::inRange(-20, 21);
        auto d1 = *gen::inRange(1, 21);
        auto n2 = *gen::inRange(-20, 21);
        auto d2 = *gen::inRange(1, 21);
        return Dq(Q(n1, d1), Q(n2, d2));
      });
    }
  };

} // namespace rc

// --- Construction tests ---

TEST_CASE("Dual default constructs to zero", "[dual]") {
  Dd z;
  CHECK(z == Dd(0.0, 0.0));
}

TEST_CASE("Dual from single value", "[dual]") {
  Dd z(3.0);
  CHECK(z.real() == 3.0);
  CHECK(z.dual() == 0.0);
}

TEST_CASE("Dual from two values", "[dual]") {
  Dd z(1.0, 2.0);
  CHECK(z.real() == 1.0);
  CHECK(z.dual() == 2.0);
}

// --- Basic arithmetic ---

TEST_CASE("Dual addition", "[dual]") {
  auto a = Dd(1.0, 2.0);
  auto b = Dd(3.0, 4.0);
  CHECK(a + b == Dd(4.0, 6.0));
}

TEST_CASE("Dual subtraction", "[dual]") {
  auto a = Dd(5.0, 3.0);
  auto b = Dd(2.0, 1.0);
  CHECK(a - b == Dd(3.0, 2.0));
}

TEST_CASE("Dual multiplication", "[dual]") {
  // (a+bε)(c+dε) = ac + (ad+bc)ε  (since ε²=0)
  auto a = Dd(2.0, 3.0);
  auto b = Dd(4.0, 5.0);
  // 2*4 = 8, 2*5 + 3*4 = 22
  CHECK(a * b == Dd(8.0, 22.0));
}

TEST_CASE("Dual epsilon squared is zero", "[dual]") {
  // ε * ε = (0+1ε)(0+1ε) = 0 + (0*1+1*0)ε = 0
  auto eps = Dd(0.0, 1.0);
  CHECK(eps * eps == Dd(0.0, 0.0));
}

// --- Negation ---

TEST_CASE("Dual negation", "[dual]") {
  auto z = Dd(1.0, -2.0);
  CHECK(-z == Dd(-1.0, 2.0));
}

// --- Automatic differentiation property ---

TEST_CASE("Dual encodes derivative of x^2", "[dual]") {
  // f(x) = x², f'(x) = 2x
  // f(3+ε) = (3+ε)² = 9 + 6ε
  auto x = Dd(3.0, 1.0);
  auto result = x * x;
  CHECK(result.real() == 9.0);
  CHECK(result.dual() == 6.0); // f'(3) = 6
}

TEST_CASE("Dual encodes derivative of x^3", "[dual]") {
  // f(x) = x³, f'(x) = 3x²
  // f(2+ε) = (2+ε)³ = 8 + 12ε
  auto x = Dd(2.0, 1.0);
  auto result = x * x * x;
  CHECK(result.real() == 8.0);
  CHECK(result.dual() == 12.0); // f'(2) = 12
}

// --- Algebraic law suites ---

TEST_CASE("Dual<double> satisfies CommutativeRing laws",
          "[laws][dual]") {
  arithkit::laws::check_commutative_ring<Dd>();
}

TEST_CASE("Dual<int> satisfies CommutativeRing laws", "[laws][dual]") {
  arithkit::laws::check_commutative_ring<Di>();
}

TEST_CASE("Dual<Fraction<int>> satisfies CommutativeRing laws",
          "[laws][dual]") {
  arithkit::laws::check_commutative_ring<Dq>();
}

TEST_CASE(
  "Dual<int> satisfies LinearAugmentation ring laws", "[laws][dual]") {
  arithkit::laws::check_linear_augmentation_ring<int>();
}

TEST_CASE(
  "Dual<double> satisfies LinearAugmentation field laws (approx)",
  "[laws][dual]") {
  arithkit::laws::check_approx_linear_augmentation_field<double>();
}

TEST_CASE(
  "Dual<Fraction<int>> satisfies LinearAugmentation field laws",
  "[laws][dual]") {
  arithkit::laws::check_linear_augmentation_field<Fraction<int>>();
}
