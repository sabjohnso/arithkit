#include <arithkit/bigint.hpp>
#include <arithkit/concepts.hpp>
#include <arithkit/fraction.hpp>
#include <arithkit/octonion.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "laws/alternative_division_algebra_laws.hpp"

using namespace arithkit;

using BigQ = Fraction<BigInt>;
using Od = Octonion<double>;
using Oq = Octonion<BigQ>;

// --- Static concept checks ---

static_assert(AlternativeDivisionAlgebra<Od>);
static_assert(AlternativeDivisionAlgebra<Oq>);

// Syntactically satisfies Ring/DivisionRing (C++20 cannot enforce
// non-associativity), but semantically is NOT associative — verified
// by counterexample below.
static_assert(NonAssociativeRing<Od>);

// Not ordered
static_assert(!std::totally_ordered<Od>);

// --- constexpr checks ---
static_assert(
  Octonion<double>(1, 0, 0, 0, 0, 0, 0, 0) +
    Octonion<double>(0, 1, 0, 0, 0, 0, 0, 0) ==
  Octonion<double>(1, 1, 0, 0, 0, 0, 0, 0));
static_assert(
  Octonion<double>(1, 0, 0, 0, 0, 0, 0, 0) ==
  Octonion<double>(1, 0, 0, 0, 0, 0, 0, 0));

// --- RapidCheck generators ---

namespace rc {

  template <>
  struct Arbitrary<Oq> {
    static Gen<Oq> arbitrary() {
      return gen::exec([] {
        auto gen_comp = [] {
          auto n = *gen::inRange(-10, 11);
          auto d = *gen::inRange(1, 11);
          return BigQ(BigInt(n), BigInt(d));
        };
        return Oq(gen_comp(), gen_comp(), gen_comp(), gen_comp(),
                   gen_comp(), gen_comp(), gen_comp(), gen_comp());
      });
    }
  };

} // namespace rc

// --- Construction tests ---

TEST_CASE("Octonion default constructs to zero", "[octonion]") {
  Od z;
  CHECK(z == Od(0, 0, 0, 0, 0, 0, 0, 0));
}

TEST_CASE("Octonion from scalar", "[octonion]") {
  Od z(3.0);
  CHECK(z[0] == 3.0);
  for (std::size_t i = 1; i < 8; ++i) {
    CHECK(z[i] == 0.0);
  }
}

TEST_CASE("Octonion from array", "[octonion]") {
  std::array<double, 8> arr = {1, 2, 3, 4, 5, 6, 7, 8};
  Od z(arr);
  for (std::size_t i = 0; i < 8; ++i) {
    CHECK(z[i] == arr[i]);
  }
}

TEST_CASE("Octonion from 8 components", "[octonion]") {
  Od z(1, 2, 3, 4, 5, 6, 7, 8);
  CHECK(z[0] == 1);
  CHECK(z[1] == 2);
  CHECK(z[7] == 8);
}

// --- Basic arithmetic ---

TEST_CASE("Octonion addition", "[octonion]") {
  auto a = Od(1, 2, 3, 4, 5, 6, 7, 8);
  auto b = Od(8, 7, 6, 5, 4, 3, 2, 1);
  CHECK(a + b == Od(9, 9, 9, 9, 9, 9, 9, 9));
}

TEST_CASE("Octonion subtraction", "[octonion]") {
  auto a = Od(8, 7, 6, 5, 4, 3, 2, 1);
  auto b = Od(1, 2, 3, 4, 5, 6, 7, 8);
  CHECK(a - b == Od(7, 5, 3, 1, -1, -3, -5, -7));
}

TEST_CASE("Octonion negation", "[octonion]") {
  auto q = Od(1, -2, 3, -4, 5, -6, 7, -8);
  CHECK(-q == Od(-1, 2, -3, 4, -5, 6, -7, 8));
}

// --- Basis element squares: e_i^2 = -1 for i = 1..7 ---

TEST_CASE("Octonion basis element squares", "[octonion]") {
  Od neg_one(-1, 0, 0, 0, 0, 0, 0, 0);

  for (std::size_t i = 1; i < 8; ++i) {
    Od ei;
    ei[i] = 1.0;
    CHECK(ei * ei == neg_one);
  }
}

// --- Fano plane triples ---
// Each triple (i,j,k) means e_i * e_j = +e_k (cyclic)
// and e_j * e_i = -e_k (anti-cyclic)

TEST_CASE("Octonion Fano plane triples (cyclic)", "[octonion]") {
  // Helper: create unit basis element
  auto basis = [](std::size_t idx) {
    Od e;
    e[idx] = 1.0;
    return e;
  };

  struct Triple {
    std::size_t i, j, k;
  };

  // The 7 Fano plane triples
  Triple triples[] = {{1, 2, 3}, {1, 4, 5}, {1, 7, 6}, {2, 4, 6},
                       {2, 5, 7}, {3, 4, 7}, {3, 6, 5}};

  for (auto [i, j, k] : triples) {
    INFO("Triple (" << i << "," << j << "," << k << ")");

    // Cyclic: e_i * e_j = +e_k
    CHECK(basis(i) * basis(j) == basis(k));
    // Cyclic: e_j * e_k = +e_i
    CHECK(basis(j) * basis(k) == basis(i));
    // Cyclic: e_k * e_i = +e_j
    CHECK(basis(k) * basis(i) == basis(j));

    // Anti-cyclic: e_j * e_i = -e_k
    CHECK(basis(j) * basis(i) == -basis(k));
    // Anti-cyclic: e_k * e_j = -e_i
    CHECK(basis(k) * basis(j) == -basis(i));
    // Anti-cyclic: e_i * e_k = -e_j
    CHECK(basis(i) * basis(k) == -basis(j));
  }
}

// --- Non-associativity witness ---

TEST_CASE("Octonion multiplication is non-associative", "[octonion]") {
  auto basis = [](std::size_t idx) {
    Od e;
    e[idx] = 1.0;
    return e;
  };

  auto e1 = basis(1);
  auto e2 = basis(2);
  auto e4 = basis(4);

  // e1 * (e2 * e4) = e1 * e6 = -e7  (because triple (2,4,6) and
  //                                    (1,7,6) means e1*e6 = -e7)
  // But wait, let's check the actual Fano table:
  // (2,4,6): e2*e4 = e6
  // (1,7,6): e1*e7 = e6, so e6*e1 = e7, and e1*e6 = -e7
  // So e1*(e2*e4) = e1*e6 = -e7
  auto left = e1 * (e2 * e4);

  // (1,2,3): e1*e2 = e3
  // (3,4,7): e3*e4 = e7
  // So (e1*e2)*e4 = e3*e4 = e7
  auto right = (e1 * e2) * e4;

  // left = -e7, right = +e7 → they differ
  CHECK_FALSE(left == right);
  CHECK(left == -right);
}

// --- Non-commutativity witness ---

TEST_CASE("Octonion multiplication is non-commutative", "[octonion]") {
  auto a = Od(1, 2, 3, 4, 5, 6, 7, 8);
  auto b = Od(8, 7, 6, 5, 4, 3, 2, 1);
  CHECK_FALSE(a * b == b * a);
}

// --- Conjugate and norm ---

TEST_CASE("Octonion conjugate", "[octonion]") {
  auto q = Od(1, 2, 3, 4, 5, 6, 7, 8);
  auto c = conjugate(q);
  CHECK(c[0] == 1);
  for (std::size_t i = 1; i < 8; ++i) {
    CHECK(c[i] == -q[i]);
  }
}

TEST_CASE("Octonion norm", "[octonion]") {
  // norm(1+2e1+3e2+...+8e7) = 1+4+9+16+25+36+49+64 = 204
  auto q = Oq(BigQ(BigInt(1)), BigQ(BigInt(2)), BigQ(BigInt(3)),
               BigQ(BigInt(4)), BigQ(BigInt(5)), BigQ(BigInt(6)),
               BigQ(BigInt(7)), BigQ(BigInt(8)));
  CHECK(norm(q) == BigQ(BigInt(204)));
}

// --- Division (exact) ---

TEST_CASE("Octonion division by itself", "[octonion]") {
  auto one = BigQ(BigInt(1));
  auto q = Oq(BigQ(BigInt(1)), BigQ(BigInt(2)), BigQ(BigInt(3)),
               BigQ(BigInt(4)), BigQ(BigInt(5)), BigQ(BigInt(6)),
               BigQ(BigInt(7)), BigQ(BigInt(8)));
  auto result = q / q;
  CHECK(result == Oq(one));
}

// --- Algebraic law suites ---

TEST_CASE(
  "Octonion<Fraction<BigInt>> satisfies AlternativeDivisionAlgebra laws",
  "[laws][octonion]") {
  arithkit::laws::check_alternative_division_algebra<Oq>();
}
