/// @file linear_transforms.cpp
/// @brief 2D linear transformations using Matrix<double,2,2>.
///
/// Demonstrates rotation, scaling, composition via matrix
/// multiplication, determinants, inverse, and solving Ax=b.

#include <cmath>
#include <iomanip>
#include <iostream>

#include <arithkit/fraction.hpp>
#include <arithkit/matrix.hpp>

using arithkit::Fraction;
using arithkit::Matrix;

using Mat2 = Matrix<double, 2, 2>;
using Vec2 = Matrix<double, 2, 1>;
using Mat2q = Matrix<Fraction<int>, 2, 2>;
using Vec2q = Matrix<Fraction<int>, 2, 1>;

/// Build a 2D rotation matrix for angle θ (radians).
auto
rotation(double theta) -> Mat2 {
  double c = std::cos(theta);
  double s = std::sin(theta);
  return Mat2(std::array<double, 4>{c, -s, s, c});
}

/// Build a 2D scaling matrix.
auto
scaling(double sx, double sy) -> Mat2 {
  return Mat2(std::array<double, 4>{sx, 0.0, 0.0, sy});
}

void
print_vec(const char* label, const Vec2& v) {
  std::cout << std::setw(20) << label << " = (" << std::setw(8)
            << std::setprecision(4) << v(0, 0) << ", " << std::setw(8)
            << v(1, 0) << ")\n";
}

int
main() {
  constexpr double pi = 3.14159265358979323846;

  std::cout << "=== 2D Linear Transforms with Matrix<double,2,2> ===\n\n";

  // --- Rotation ---
  auto R = rotation(pi / 4); // 45 degrees
  auto point = Vec2(std::array<double, 2>{1.0, 0.0});
  auto rotated = arithkit::matmul(R, point);

  std::cout << "Rotation by 45 degrees:\n";
  print_vec("(1,0) ->", rotated);
  std::cout << "Expected: (" << std::setw(8) << std::cos(pi / 4) << ", "
            << std::setw(8) << std::sin(pi / 4) << ")\n\n";

  // --- Scaling ---
  auto S = scaling(2.0, 3.0);
  auto scaled = arithkit::matmul(S, point);

  std::cout << "Scaling by (2,3):\n";
  print_vec("(1,0) ->", scaled);
  std::cout << "\n";

  // --- Composition: scale then rotate ---
  auto T = arithkit::matmul(R, S); // R * S: first scale, then rotate
  auto composed = arithkit::matmul(T, point);

  std::cout << "Composition (scale then rotate):\n";
  print_vec("(1,0) ->", composed);
  std::cout << "\n";

  // --- Determinants ---
  std::cout << "Determinants:\n";
  std::cout << "  det(R_45)   = " << arithkit::determinant(R)
            << "  (rotation preserves area)\n";
  std::cout << "  det(S_2x3)  = " << arithkit::determinant(S)
            << "  (area scales by 6)\n";
  std::cout << "  det(R*S)    = " << arithkit::determinant(T)
            << "  (= det(R)*det(S))\n\n";

  // --- Exact inverse over Fraction<int> ---
  using Q = Fraction<int>;
  std::cout << "--- Exact inverse over Fraction<int> ---\n";
  auto A = Mat2q(std::array<Q, 4>{Q(2), Q(1), Q(5), Q(3)});
  auto A_inv = arithkit::inverse(A);
  auto I = arithkit::identity_v<Mat2q, arithkit::multiplicative_tag>();

  std::cout << "A     = " << A << "\n";
  std::cout << "A^-1  = " << A_inv << "\n";
  std::cout << "A*A^-1 = " << A * A_inv << "  (should be I)\n";
  std::cout << "Verified: " << (A * A_inv == I ? "yes" : "NO") << "\n\n";

  // --- Solve Ax = b via x = A^-1 * b ---
  std::cout << "--- Solve Ax = b ---\n";
  auto b = Vec2q(std::array<Q, 2>{Q(7), Q(19)});
  auto x = arithkit::matmul(A_inv, b);

  std::cout << "A = " << A << "\n";
  std::cout << "b = " << b << "\n";
  std::cout << "x = A^-1 * b = " << x << "\n";
  std::cout << "Verify: A*x = " << arithkit::matmul(A, x) << "  (should be b)\n";

  return 0;
}
