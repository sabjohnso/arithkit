/// @file rotation_3d.cpp
/// @brief 3D rotation using Quaternion<double>.
///
/// Represents rotations as unit quaternions and applies them to 3D
/// points using the sandwich product: p' = q * p * q^(-1).
/// Demonstrates rotation composition by multiplying quaternions
/// (avoiding gimbal lock and matrix overhead).

#include <cmath>
#include <iomanip>
#include <iostream>

#include <arithkit/quaternion.hpp>

using arithkit::Quaternion;
using Q = Quaternion<double>;

/// Construct a unit quaternion representing a rotation of `angle`
/// radians around the unit axis (ax, ay, az).
auto
rotation_quat(double angle, double ax, double ay, double az) -> Q {
  double half = angle / 2.0;
  double s = std::sin(half);
  return Q(std::cos(half), ax * s, ay * s, az * s);
}

/// Rotate a 3D point (x,y,z) by quaternion q using the sandwich
/// product: p' = q * p * conj(q).
/// The point is embedded as a pure quaternion (0, x, y, z).
struct Vec3 {
  double x, y, z;
};

auto
rotate(const Q& q, const Vec3& p) -> Vec3 {
  Q pure(0.0, p.x, p.y, p.z);
  auto rotated = q * pure * conjugate(q);
  return {rotated.x(), rotated.y(), rotated.z()};
}

void
print_vec(const char* label, const Vec3& v) {
  std::cout << std::setw(24) << label << " = (" << std::setw(8)
            << std::setprecision(4) << v.x << ", " << std::setw(8) << v.y
            << ", " << std::setw(8) << v.z << ")\n";
}

int
main() {
  constexpr double pi = 3.14159265358979323846;

  std::cout << "=== 3D Rotation with Quaternion<double> ===\n\n";

  // Rotate point (1,0,0) by 90 degrees around the Z axis
  // Expected: (0,1,0)
  auto q_z90 = rotation_quat(pi / 2, 0, 0, 1);
  Vec3 p = {1.0, 0.0, 0.0};
  auto p1 = rotate(q_z90, p);

  std::cout << "Rotation: 90 degrees around Z axis\n";
  print_vec("(1,0,0) ->", p1);
  std::cout << "Expected: (   0,        1,        0)\n\n";

  // Rotate (0,1,0) by 90 degrees around X axis -> (0,0,1)
  auto q_x90 = rotation_quat(pi / 2, 1, 0, 0);
  Vec3 p2_in = {0.0, 1.0, 0.0};
  auto p2 = rotate(q_x90, p2_in);

  std::cout << "Rotation: 90 degrees around X axis\n";
  print_vec("(0,1,0) ->", p2);
  std::cout << "Expected: (   0,        0,        1)\n\n";

  // Composition: Z90 then X90 = single quaternion
  auto q_composed = q_x90 * q_z90;
  Vec3 p3_in = {1.0, 0.0, 0.0};

  // Apply individually
  auto step1 = rotate(q_z90, p3_in);
  auto step2 = rotate(q_x90, step1);

  // Apply composed
  auto composed_result = rotate(q_composed, p3_in);

  std::cout << "Composition: Z90 then X90 on (1,0,0)\n";
  print_vec("Step-by-step", step2);
  print_vec("Composed (q2*q1)", composed_result);
  std::cout << "\nComposition works: quaternion multiplication\n"
            << "composes rotations without gimbal lock.\n\n";

  // Rotate a triangle
  std::cout << "--- Rotating a triangle 45 degrees around (1,1,1)/sqrt(3) ---\n";
  double inv_sqrt3 = 1.0 / std::sqrt(3.0);
  auto q_diag = rotation_quat(pi / 4, inv_sqrt3, inv_sqrt3, inv_sqrt3);

  Vec3 triangle[] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
  for (auto& v : triangle) {
    auto rv = rotate(q_diag, v);
    std::cout << "  (" << std::setw(6) << v.x << "," << std::setw(6) << v.y
              << "," << std::setw(6) << v.z << ") -> (" << std::setw(8)
              << rv.x << "," << std::setw(8) << rv.y << "," << std::setw(8)
              << rv.z << ")\n";
  }

  // Verify unit quaternion property is preserved through composition
  double n1 = norm(q_z90);
  double n2 = norm(q_composed);
  std::cout << "\nUnit quaternion check:\n";
  std::cout << "  |q_z90| = " << n1 << "  (should be 1)\n";
  std::cout << "  |q_composed| = " << n2 << "  (should be 1)\n";

  return 0;
}
