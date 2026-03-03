#include <arithkit/concepts.hpp>
#include <arithkit/grassmann.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "laws/grassmann_algebra_laws.hpp"
#include "laws/ring_laws.hpp"

using namespace arithkit;

// Grassmann<T,n> = exterior algebra ∧(Tⁿ), dimension 2^n
using G1 = Grassmann<double, 1>; // dim 2: {1, e₁}
using G2 = Grassmann<double, 2>; // dim 4: {1, e₁, e₂, e₁₂}
using G3 = Grassmann<double, 3>; // dim 8

// --- Static concept checks ---

static_assert(Ring<G1>);
static_assert(Ring<G2>);
static_assert(Ring<G3>);

// --- constexpr checks ---
static_assert(
  Grassmann<double, 2>(1.0) + Grassmann<double, 2>(2.0) ==
  Grassmann<double, 2>(3.0));
static_assert(Grassmann<double, 2>() == Grassmann<double, 2>(0.0));

// --- RapidCheck generators ---

namespace rc {

  template <>
  struct Arbitrary<G1> {
    static Gen<G1> arbitrary() {
      return gen::exec([] {
        std::array<double, 2> c;
        for (auto& v : c)
          v = double(*gen::inRange(-50, 51));
        return G1(c);
      });
    }
  };

  template <>
  struct Arbitrary<G2> {
    static Gen<G2> arbitrary() {
      return gen::exec([] {
        std::array<double, 4> c;
        for (auto& v : c)
          v = double(*gen::inRange(-20, 21));
        return G2(c);
      });
    }
  };

  template <>
  struct Arbitrary<G3> {
    static Gen<G3> arbitrary() {
      return gen::exec([] {
        std::array<double, 8> c;
        for (auto& v : c)
          v = double(*gen::inRange(-10, 11));
        return G3(c);
      });
    }
  };

} // namespace rc

// --- Construction tests ---

TEST_CASE("Grassmann default constructs to zero", "[grassmann]") {
  G2 z;
  for (std::size_t i = 0; i < G2::dim(); ++i) {
    CHECK(z[i] == 0.0);
  }
}

TEST_CASE("Grassmann from scalar", "[grassmann]") {
  G2 z(3.0);
  CHECK(z[0] == 3.0);
  CHECK(z[1] == 0.0);
  CHECK(z[2] == 0.0);
  CHECK(z[3] == 0.0);
}

// --- Anticommutativity ---

TEST_CASE("Grassmann eᵢ∧eᵢ = 0", "[grassmann]") {
  auto e1 = G2(std::array{0.0, 1.0, 0.0, 0.0});
  auto e2 = G2(std::array{0.0, 0.0, 1.0, 0.0});

  // eᵢ∧eᵢ = 0
  auto e1_sq = e1 * e1;
  CHECK(e1_sq[0] == 0.0);
  CHECK(e1_sq[1] == 0.0);
  CHECK(e1_sq[2] == 0.0);
  CHECK(e1_sq[3] == 0.0);

  auto e2_sq = e2 * e2;
  CHECK(e2_sq[0] == 0.0);
  CHECK(e2_sq[1] == 0.0);
  CHECK(e2_sq[2] == 0.0);
  CHECK(e2_sq[3] == 0.0);
}

TEST_CASE("Grassmann eᵢ∧eⱼ = −eⱼ∧eᵢ", "[grassmann]") {
  auto e1 = G2(std::array{0.0, 1.0, 0.0, 0.0});
  auto e2 = G2(std::array{0.0, 0.0, 1.0, 0.0});

  auto e1e2 = e1 * e2;
  auto e2e1 = e2 * e1;

  // e₁∧e₂ + e₂∧e₁ = 0
  CHECK(e1e2[3] == 1.0);
  CHECK(e2e1[3] == -1.0);
}

TEST_CASE("Grassmann wedge product", "[grassmann]") {
  // (2 + 3e₁) ∧ (1 + 4e₂) = 2 + 8e₂ + 3e₁ + 12e₁₂
  auto a = G2(std::array{2.0, 3.0, 0.0, 0.0});
  auto b = G2(std::array{1.0, 0.0, 4.0, 0.0});
  auto c = a * b;
  CHECK(c[0] == 2.0);   // scalar: 2*1
  CHECK(c[1] == 3.0);   // e₁: 3*1
  CHECK(c[2] == 8.0);   // e₂: 2*4
  CHECK(c[3] == 12.0);  // e₁₂: 3*4
}

// --- Negation ---

TEST_CASE("Grassmann negation", "[grassmann]") {
  auto a = G2(std::array{1.0, -2.0, 3.0, -4.0});
  CHECK(-a == G2(std::array{-1.0, 2.0, -3.0, 4.0}));
}

// --- Grade 3 ---

TEST_CASE("Grassmann<3> e₁∧e₂∧e₃", "[grassmann]") {
  auto e1 = G3(std::array{0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0});
  auto e2 = G3(std::array{0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0});
  auto e3 = G3(std::array{0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0});
  auto e123 = e1 * e2 * e3;
  // e₁₂₃ corresponds to bitmask 0b111 = 7
  CHECK(e123[7] == 1.0);
}

// --- Algebraic law suites ---

TEST_CASE("Grassmann<1> satisfies Ring laws", "[laws][grassmann]") {
  arithkit::laws::check_ring<G1>();
}

TEST_CASE("Grassmann<2> satisfies Ring laws", "[laws][grassmann]") {
  arithkit::laws::check_ring<G2>();
}

TEST_CASE("Grassmann<3> satisfies Ring laws", "[laws][grassmann]") {
  arithkit::laws::check_ring<G3>();
}

TEST_CASE("Grassmann<1> satisfies Grassmann algebra laws",
          "[laws][grassmann]") {
  arithkit::laws::check_grassmann_algebra<double, 1>();
}

TEST_CASE("Grassmann<2> satisfies Grassmann algebra laws",
          "[laws][grassmann]") {
  arithkit::laws::check_grassmann_algebra<double, 2>();
}

TEST_CASE("Grassmann<3> satisfies Grassmann algebra laws",
          "[laws][grassmann]") {
  arithkit::laws::check_grassmann_algebra<double, 3>();
}
