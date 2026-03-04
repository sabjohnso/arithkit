#include <arithkit/concepts.hpp>
#include <arithkit/fraction.hpp>
#include <arithkit/polynomial.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "laws/commutative_ring_laws.hpp"
#include "laws/euclidean_domain_laws.hpp"

using namespace arithkit;

using Pi = Polynomial<int>;
using Pd = Polynomial<double>;
using Pq = Polynomial<Fraction<int>>;

// --- Static concept checks ---

static_assert(CommutativeRing<Pi>);
static_assert(CommutativeRing<Pd>);
static_assert(CommutativeRing<Pq>);

// EuclideanDomain when T is a Field (has division)
static_assert(EuclideanDomain<Pd>);
static_assert(EuclideanDomain<Pq>);

// int has operator/ and operator%, so Polynomial<int> also satisfies
// EuclideanDomain syntactically (like int itself).
static_assert(EuclideanDomain<Pi>);

// Note: Field<Polynomial<T>> is syntactically true when Field<T> is
// true (which includes int and double).  Polynomial is not a true field
// (degree >= 1 elements have no inverse), but this is enforced by
// property tests, not by the type system — same gotcha as Field<int>.

// --- RapidCheck generators ---

namespace rc {

  template <>
  struct Arbitrary<Pi> {
    static Gen<Pi> arbitrary() {
      return gen::exec([] {
        auto len = *gen::inRange(0, 5);
        std::vector<int> coeffs(static_cast<std::size_t>(len));
        for (auto& c : coeffs) c = *gen::inRange(-20, 21);
        return Pi(std::move(coeffs));
      });
    }
  };

  template <>
  struct Arbitrary<Pd> {
    static Gen<Pd> arbitrary() {
      return gen::exec([] {
        auto len = *gen::inRange(0, 5);
        std::vector<double> coeffs(static_cast<std::size_t>(len));
        for (auto& c : coeffs) c = double(*gen::inRange(-20, 21));
        return Pd(std::move(coeffs));
      });
    }
  };

  template <>
  struct Arbitrary<Pq> {
    static Gen<Pq> arbitrary() {
      return gen::exec([] {
        using Q = Fraction<int>;
        auto len = *gen::inRange(1, 4);
        std::vector<Q> coeffs(static_cast<std::size_t>(len));
        for (auto& c : coeffs) {
          auto n = *gen::inRange(-10, 11);
          auto d = *gen::inRange(1, 11);
          c = Q(n, d);
        }
        return Pq(std::move(coeffs));
      });
    }
  };

} // namespace rc

// --- Construction tests ---

TEST_CASE("Polynomial default constructs to zero", "[polynomial]") {
  Pi z;
  CHECK(z == Pi());
  CHECK(z.degree() == 0);
  CHECK(z.is_zero());
}

TEST_CASE("Polynomial from scalar", "[polynomial]") {
  Pi p(5);
  CHECK(p.degree() == 0);
  CHECK(p[0] == 5);
  CHECK(!p.is_zero());
}

TEST_CASE("Polynomial from initializer list", "[polynomial]") {
  // 1 + 2x + 3x^2
  Pi p({1, 2, 3});
  CHECK(p.degree() == 2);
  CHECK(p[0] == 1);
  CHECK(p[1] == 2);
  CHECK(p[2] == 3);
}

TEST_CASE("Polynomial normalizes trailing zeros", "[polynomial]") {
  Pi p({1, 2, 0, 0});
  CHECK(p.degree() == 1);
  CHECK(p == Pi({1, 2}));
}

TEST_CASE("Polynomial zero from all-zero coefficients", "[polynomial]") {
  Pi p({0, 0, 0});
  CHECK(p.is_zero());
  CHECK(p.degree() == 0);
}

// --- Basic arithmetic ---

TEST_CASE("Polynomial addition", "[polynomial]") {
  Pi a({1, 2, 3});  // 1 + 2x + 3x²
  Pi b({4, 5});      // 4 + 5x
  CHECK(a + b == Pi({5, 7, 3}));
}

TEST_CASE("Polynomial subtraction", "[polynomial]") {
  Pi a({5, 7, 3});
  Pi b({4, 5});
  CHECK(a - b == Pi({1, 2, 3}));
}

TEST_CASE("Polynomial multiplication", "[polynomial]") {
  // (1 + 2x)(3 + 4x) = 3 + 10x + 8x²
  Pi a({1, 2});
  Pi b({3, 4});
  CHECK(a * b == Pi({3, 10, 8}));
}

TEST_CASE("Polynomial multiply by zero", "[polynomial]") {
  Pi a({1, 2, 3});
  Pi zero;
  CHECK(a * zero == zero);
}

TEST_CASE("Polynomial negation", "[polynomial]") {
  Pi a({1, -2, 3});
  CHECK(-a == Pi({-1, 2, -3}));
}

// --- Coefficient access ---

TEST_CASE("Polynomial bracket returns zero beyond degree",
          "[polynomial]") {
  Pi a({1, 2});
  CHECK(a[0] == 1);
  CHECK(a[1] == 2);
  CHECK(a[5] == 0);  // beyond degree
}

// --- Evaluation (Horner's method) ---

TEST_CASE("Polynomial evaluate", "[polynomial]") {
  // 1 + 2x + 3x² at x=2: 1 + 4 + 12 = 17
  Pi p({1, 2, 3});
  CHECK(p.evaluate(2) == 17);
}

TEST_CASE("Polynomial evaluate zero polynomial", "[polynomial]") {
  Pi p;
  CHECK(p.evaluate(42) == 0);
}

// --- Division and remainder (over Fraction field) ---

TEST_CASE("Polynomial division over Fraction<int>", "[polynomial]") {
  using Q = Fraction<int>;
  // (6 + 5x + x²) / (2 + x) = (3 + x) remainder 0? Let's check:
  // (2+x)(3+x) = 6 + 5x + x² ✓
  Pq a({Q(6), Q(5), Q(1)});
  Pq b({Q(2), Q(1)});
  CHECK(a / b == Pq({Q(3), Q(1)}));
  CHECK(a % b == Pq());
}

TEST_CASE("Polynomial division with remainder", "[polynomial]") {
  using Q = Fraction<int>;
  // (1 + x + x²) / (1 + x) = x, remainder 1
  // because (1+x)*x + 1 = x + x² + 1 ✓
  Pq a({Q(1), Q(1), Q(1)});
  Pq b({Q(1), Q(1)});
  auto q = a / b;
  auto r = a % b;
  CHECK(q == Pq({Q(0), Q(1)}));
  CHECK(r == Pq({Q(1)}));
  CHECK(a == q * b + r);
}

// --- Mixed scalar operators ---

TEST_CASE("Polynomial mixed scalar operators", "[polynomial]") {
  Pi p({1, 2, 3});
  CHECK(p + 10 == Pi({11, 2, 3}));
  CHECK(10 + p == Pi({11, 2, 3}));
  CHECK(p * 2 == Pi({2, 4, 6}));
  CHECK(2 * p == Pi({2, 4, 6}));
  CHECK(p - 1 == Pi({0, 2, 3}));
  CHECK(10 - p == Pi({9, -2, -3}));
}

// --- Algebraic law suites ---

TEST_CASE("Polynomial<int> satisfies CommutativeRing laws",
          "[laws][polynomial]") {
  arithkit::laws::check_commutative_ring<Pi>();
}

TEST_CASE("Polynomial<Fraction<int>> satisfies CommutativeRing laws",
          "[laws][polynomial]") {
  arithkit::laws::check_commutative_ring<Pq>();
}

TEST_CASE(
  "Polynomial<Fraction<int>> satisfies EuclideanDomain laws",
  "[laws][polynomial]") {
  arithkit::laws::check_euclidean_domain<Pq>();
}
