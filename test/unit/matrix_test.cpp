#include <arithkit/concepts.hpp>
#include <arithkit/fraction.hpp>
#include <arithkit/matrix.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "laws/ring_laws.hpp"

using namespace arithkit;

// Convenient type aliases
using Mat2i = Matrix<int, 2, 2>;
using Mat3i = Matrix<int, 3, 3>;
using Mat23i = Matrix<int, 2, 3>;
using Mat32i = Matrix<int, 3, 2>;
using Mat2d = Matrix<double, 2, 2>;
using Frac = Fraction<int>;
using Mat2q = Matrix<Frac, 2, 2>;
using Mat3q = Matrix<Frac, 3, 3>;
using Vec2i = Matrix<int, 2, 1>;
using Vec3i = Matrix<int, 3, 1>;

// --- Static concept assertions ---

static_assert(Ring<Mat2i>);
static_assert(Ring<Mat3i>);
static_assert(!Ring<Mat23i>); // rectangular — no multiplicative monoid

// --- constexpr checks ---

static_assert(
  Mat2i(std::array<int, 4>{1, 2, 3, 4}) +
    Mat2i(std::array<int, 4>{5, 6, 7, 8}) ==
  Mat2i(std::array<int, 4>{6, 8, 10, 12}));

static_assert(Mat2i() == Mat2i(std::array<int, 4>{0, 0, 0, 0}));

// --- RapidCheck generators ---

namespace rc {

  template <>
  struct Arbitrary<Mat2i> {
    static Gen<Mat2i> arbitrary() {
      return gen::exec([] {
        std::array<int, 4> data;
        for (auto& x : data) {
          x = *gen::inRange(-10, 11);
        }
        return Mat2i(data);
      });
    }
  };

  template <>
  struct Arbitrary<Mat2q> {
    static Gen<Mat2q> arbitrary() {
      return gen::exec([] {
        auto gen_frac = [] {
          auto n = *gen::inRange(-5, 6);
          auto d = *gen::inRange(1, 6);
          return Frac(n, d);
        };
        std::array<Frac, 4> data;
        for (auto& x : data) {
          x = gen_frac();
        }
        return Mat2q(data);
      });
    }
  };

  template <>
  struct Arbitrary<Mat3q> {
    static Gen<Mat3q> arbitrary() {
      return gen::exec([] {
        auto gen_frac = [] {
          auto n = *gen::inRange(-5, 6);
          auto d = *gen::inRange(1, 6);
          return Frac(n, d);
        };
        std::array<Frac, 9> data;
        for (auto& x : data) {
          x = gen_frac();
        }
        return Mat3q(data);
      });
    }
  };

} // namespace rc

// --- Construction tests ---

TEST_CASE("Matrix default constructs to zero", "[matrix]") {
  Mat2i z;
  CHECK(z(0, 0) == 0);
  CHECK(z(0, 1) == 0);
  CHECK(z(1, 0) == 0);
  CHECK(z(1, 1) == 0);
}

TEST_CASE("Matrix from flat array", "[matrix]") {
  auto m = Mat2i(std::array<int, 4>{1, 2, 3, 4});
  CHECK(m(0, 0) == 1);
  CHECK(m(0, 1) == 2);
  CHECK(m(1, 0) == 3);
  CHECK(m(1, 1) == 4);
}

TEST_CASE("Matrix from scalar (square only)", "[matrix]") {
  auto m = Mat2i(5);
  CHECK(m(0, 0) == 5);
  CHECK(m(0, 1) == 0);
  CHECK(m(1, 0) == 0);
  CHECK(m(1, 1) == 5);
}

// --- Component-wise arithmetic ---

TEST_CASE("Matrix addition", "[matrix]") {
  auto a = Mat2i(std::array<int, 4>{1, 2, 3, 4});
  auto b = Mat2i(std::array<int, 4>{5, 6, 7, 8});
  CHECK(a + b == Mat2i(std::array<int, 4>{6, 8, 10, 12}));
}

TEST_CASE("Matrix subtraction", "[matrix]") {
  auto a = Mat2i(std::array<int, 4>{5, 6, 7, 8});
  auto b = Mat2i(std::array<int, 4>{1, 2, 3, 4});
  CHECK(a - b == Mat2i(std::array<int, 4>{4, 4, 4, 4}));
}

TEST_CASE("Matrix unary negation", "[matrix]") {
  auto m = Mat2i(std::array<int, 4>{1, -2, 3, -4});
  CHECK(-m == Mat2i(std::array<int, 4>{-1, 2, -3, 4}));
}

TEST_CASE("Matrix scalar multiplication", "[matrix]") {
  auto m = Mat2i(std::array<int, 4>{1, 2, 3, 4});
  CHECK(m * 3 == Mat2i(std::array<int, 4>{3, 6, 9, 12}));
  CHECK(3 * m == Mat2i(std::array<int, 4>{3, 6, 9, 12}));
}

TEST_CASE("Matrix scalar division", "[matrix]") {
  auto m = Mat2d(std::array<double, 4>{2.0, 4.0, 6.0, 8.0});
  auto expected = Mat2d(std::array<double, 4>{1.0, 2.0, 3.0, 4.0});
  CHECK(m / 2.0 == expected);
}

// --- Matrix multiplication (ring operator*) ---

TEST_CASE("Matrix multiplication 2x2", "[matrix]") {
  // [1 2] * [5 6] = [1*5+2*7  1*6+2*8] = [19 22]
  // [3 4]   [7 8]   [3*5+4*7  3*6+4*8]   [43 50]
  auto a = Mat2i(std::array<int, 4>{1, 2, 3, 4});
  auto b = Mat2i(std::array<int, 4>{5, 6, 7, 8});
  CHECK(a * b == Mat2i(std::array<int, 4>{19, 22, 43, 50}));
}

TEST_CASE("Matrix multiplication by identity", "[matrix]") {
  auto m = Mat2i(std::array<int, 4>{1, 2, 3, 4});
  auto I = identity_v<Mat2i, multiplicative_tag>();
  CHECK(m * I == m);
  CHECK(I * m == m);
}

// --- General matmul (rectangular) ---

TEST_CASE("Rectangular matmul", "[matrix]") {
  // [1 2 3] * [7 ]   [1*7+2*8+3*9]   [50 ]
  // [4 5 6]   [8 ] = [4*7+5*8+6*9] = [122]
  //            [9 ]
  auto a = Mat23i(std::array<int, 6>{1, 2, 3, 4, 5, 6});
  auto b = Vec3i(std::array<int, 3>{7, 8, 9});
  auto result = matmul(a, b);
  CHECK(result(0, 0) == 50);
  CHECK(result(1, 0) == 122);
}

TEST_CASE("Matmul M×K × K×N", "[matrix]") {
  // [1 2 3] * [1 2] = [1+6+15   2+8+18]   = [22  28]
  // [4 5 6]   [3 4]   [4+15+30  8+20+36]     [49  64]
  //            [5 6]
  auto a = Mat23i(std::array<int, 6>{1, 2, 3, 4, 5, 6});
  auto b = Mat32i(std::array<int, 6>{1, 2, 3, 4, 5, 6});
  auto result = matmul(a, b);
  CHECK(result(0, 0) == 22);
  CHECK(result(0, 1) == 28);
  CHECK(result(1, 0) == 49);
  CHECK(result(1, 1) == 64);
}

// --- Transpose ---

TEST_CASE("Transpose 2x3", "[matrix]") {
  auto m = Mat23i(std::array<int, 6>{1, 2, 3, 4, 5, 6});
  auto t = transpose(m);
  // t is 3x2: [1 4; 2 5; 3 6]
  CHECK(t(0, 0) == 1);
  CHECK(t(0, 1) == 4);
  CHECK(t(1, 0) == 2);
  CHECK(t(1, 1) == 5);
  CHECK(t(2, 0) == 3);
  CHECK(t(2, 1) == 6);
}

TEST_CASE("Double transpose is identity", "[matrix]") {
  auto m = Mat23i(std::array<int, 6>{1, 2, 3, 4, 5, 6});
  CHECK(transpose(transpose(m)) == m);
}

// --- Determinant ---

TEST_CASE("Determinant 1x1", "[matrix]") {
  auto m = Matrix<int, 1, 1>(std::array<int, 1>{7});
  CHECK(determinant(m) == 7);
}

TEST_CASE("Determinant 2x2", "[matrix]") {
  // det([1 2; 3 4]) = 1*4 - 2*3 = -2
  auto m = Mat2i(std::array<int, 4>{1, 2, 3, 4});
  CHECK(determinant(m) == -2);
}

TEST_CASE("Determinant 3x3", "[matrix]") {
  // det([1 2 3; 4 5 6; 7 8 0]) = 1(0-48) - 2(0-42) + 3(32-35)
  //                             = -48 + 84 - 9 = 27
  auto m = Mat3i(std::array<int, 9>{1, 2, 3, 4, 5, 6, 7, 8, 0});
  CHECK(determinant(m) == 27);
}

TEST_CASE("Determinant of identity is 1", "[matrix]") {
  auto I2 = identity_v<Mat2i, multiplicative_tag>();
  auto I3 = identity_v<Mat3i, multiplicative_tag>();
  CHECK(determinant(I2) == 1);
  CHECK(determinant(I3) == 1);
}

TEST_CASE("det(A*B) == det(A)*det(B)", "[matrix][laws]") {
  rc::prop("det(A*B) == det(A)*det(B)", [](Mat2q a, Mat2q b) {
    RC_ASSERT(determinant(a * b) == determinant(a) * determinant(b));
  });
}

// --- Trace ---

TEST_CASE("Trace is sum of diagonal", "[matrix]") {
  auto m = Mat3i(std::array<int, 9>{1, 2, 3, 4, 5, 6, 7, 8, 9});
  CHECK(trace(m) == 1 + 5 + 9);
}

// --- Inverse ---

TEST_CASE("Inverse 2x2 over Fraction", "[matrix]") {
  // A = [1 2; 3 4], det = -2
  // inv = 1/det * [4 -2; -3 1] = [-2 1; 3/2 -1/2]
  auto a = Mat2q(std::array<Frac, 4>{Frac(1), Frac(2), Frac(3), Frac(4)});
  auto I = identity_v<Mat2q, multiplicative_tag>();
  auto inv_a = inverse(a);
  CHECK(a * inv_a == I);
  CHECK(inv_a * a == I);
}

TEST_CASE("Inverse 3x3 over Fraction", "[matrix]") {
  auto a = Mat3q(std::array<Frac, 9>{
    Frac(1), Frac(2), Frac(3),
    Frac(4), Frac(5), Frac(6),
    Frac(7), Frac(8), Frac(0)});
  auto I = identity_v<Mat3q, multiplicative_tag>();
  auto inv_a = inverse(a);
  CHECK(a * inv_a == I);
  CHECK(inv_a * a == I);
}

// --- Non-commutativity ---

TEST_CASE("Matrix multiplication is non-commutative", "[matrix]") {
  auto a = Mat2i(std::array<int, 4>{1, 2, 3, 4});
  auto b = Mat2i(std::array<int, 4>{5, 6, 7, 8});
  CHECK_FALSE(a * b == b * a);
}

// --- Ring law suites ---

TEST_CASE("Matrix<int,2,2> satisfies Ring laws", "[laws][matrix]") {
  arithkit::laws::check_ring<Mat2i>();
}

TEST_CASE("Matrix<Fraction<int>,2,2> satisfies Ring laws",
          "[laws][matrix]") {
  arithkit::laws::check_ring<Mat2q>();
}
