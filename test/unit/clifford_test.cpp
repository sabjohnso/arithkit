#include <arithkit/clifford.hpp>
#include <arithkit/concepts.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "laws/ring_laws.hpp"

using namespace arithkit;

// Cl(0,1) ≅ Complex: e₁² = -1
using Cl01 = Clifford<double, 0, 1>;
// Cl(1,0): e₁² = +1 (split-complex / hyperbolic numbers)
using Cl10 = Clifford<double, 1, 0>;
// Cl(0,2) ≅ Quaternions: e₁²=-1, e₂²=-1
using Cl02 = Clifford<double, 0, 2>;
// Cl(2,0): e₁²=+1, e₂²=+1
using Cl20 = Clifford<double, 2, 0>;

// --- Static concept checks ---

static_assert(Ring<Cl01>);
static_assert(Ring<Cl02>);
static_assert(Ring<Cl10>);

// --- constexpr checks ---
static_assert(
  Clifford<double, 1, 0>(1.0) + Clifford<double, 1, 0>(2.0) ==
  Clifford<double, 1, 0>(3.0));
static_assert(Clifford<double, 1, 0>() == Clifford<double, 1, 0>(0.0));

// --- RapidCheck generators ---

namespace rc {

  template <>
  struct Arbitrary<Cl01> {
    static Gen<Cl01> arbitrary() {
      return gen::exec([] {
        std::array<double, 2> c;
        for (auto& v : c)
          v = double(*gen::inRange(-50, 51));
        return Cl01(c);
      });
    }
  };

  template <>
  struct Arbitrary<Cl10> {
    static Gen<Cl10> arbitrary() {
      return gen::exec([] {
        std::array<double, 2> c;
        for (auto& v : c)
          v = double(*gen::inRange(-50, 51));
        return Cl10(c);
      });
    }
  };

  template <>
  struct Arbitrary<Cl02> {
    static Gen<Cl02> arbitrary() {
      return gen::exec([] {
        std::array<double, 4> c;
        for (auto& v : c)
          v = double(*gen::inRange(-20, 21));
        return Cl02(c);
      });
    }
  };

  template <>
  struct Arbitrary<Cl20> {
    static Gen<Cl20> arbitrary() {
      return gen::exec([] {
        std::array<double, 4> c;
        for (auto& v : c)
          v = double(*gen::inRange(-20, 21));
        return Cl20(c);
      });
    }
  };

} // namespace rc

// --- Construction tests ---

TEST_CASE("Clifford default constructs to zero", "[clifford]") {
  Cl01 z;
  CHECK(z[0] == 0.0);
  CHECK(z[1] == 0.0);
}

TEST_CASE("Clifford from scalar", "[clifford]") {
  Cl01 z(3.0);
  CHECK(z[0] == 3.0);
  CHECK(z[1] == 0.0);
}

// --- Cl(0,1) ≅ Complex ---

TEST_CASE("Cl(0,1) multiplication matches complex", "[clifford]") {
  // (1+2e₁)(3+4e₁) with e₁²=-1
  // = 3 + 4e₁ + 6e₁ + 8e₁² = 3 + 10e₁ - 8 = -5 + 10e₁
  auto a = Cl01(std::array{1.0, 2.0});
  auto b = Cl01(std::array{3.0, 4.0});
  auto c = a * b;
  CHECK(c[0] == -5.0);
  CHECK(c[1] == 10.0);
}

TEST_CASE("Cl(0,1) e₁² = -1", "[clifford]") {
  auto e1 = Cl01(std::array{0.0, 1.0});
  auto result = e1 * e1;
  CHECK(result[0] == -1.0);
  CHECK(result[1] == 0.0);
}

// --- Cl(1,0): split-complex ---

TEST_CASE("Cl(1,0) e₁² = +1", "[clifford]") {
  auto e1 = Cl10(std::array{0.0, 1.0});
  auto result = e1 * e1;
  CHECK(result[0] == 1.0);
  CHECK(result[1] == 0.0);
}

// --- Cl(0,2) ≅ Quaternions ---

TEST_CASE("Cl(0,2) basis products match quaternion rules",
          "[clifford]") {
  // Cl(0,2) has basis: 1, e₁, e₂, e₁₂
  // e₁²=-1, e₂²=-1, e₁e₂ = e₁₂, e₂e₁ = -e₁₂
  // e₁₂² = e₁e₂e₁e₂ = -e₁e₁e₂e₂ = -(−1)(−1) = -1

  auto e1 = Cl02(std::array{0.0, 1.0, 0.0, 0.0});
  auto e2 = Cl02(std::array{0.0, 0.0, 1.0, 0.0});
  auto e12 = Cl02(std::array{0.0, 0.0, 0.0, 1.0});

  // e₁² = -1
  auto e1_sq = e1 * e1;
  CHECK(e1_sq[0] == -1.0);

  // e₂² = -1
  auto e2_sq = e2 * e2;
  CHECK(e2_sq[0] == -1.0);

  // e₁e₂ = e₁₂
  auto e1e2 = e1 * e2;
  CHECK(e1e2[3] == 1.0);

  // e₂e₁ = -e₁₂
  auto e2e1 = e2 * e1;
  CHECK(e2e1[3] == -1.0);

  // e₁₂² = -1
  auto e12_sq = e12 * e12;
  CHECK(e12_sq[0] == -1.0);
}

// --- Negation ---

TEST_CASE("Clifford negation", "[clifford]") {
  auto a = Cl01(std::array{3.0, -2.0});
  CHECK(-a == Cl01(std::array{-3.0, 2.0}));
}

// --- Algebraic law suites ---

TEST_CASE("Cl(0,1) satisfies Ring laws", "[laws][clifford]") {
  arithkit::laws::check_ring<Cl01>();
}

TEST_CASE("Cl(1,0) satisfies Ring laws", "[laws][clifford]") {
  arithkit::laws::check_ring<Cl10>();
}

TEST_CASE("Cl(0,2) satisfies Ring laws", "[laws][clifford]") {
  arithkit::laws::check_ring<Cl02>();
}

TEST_CASE("Cl(2,0) satisfies Ring laws", "[laws][clifford]") {
  arithkit::laws::check_ring<Cl20>();
}
