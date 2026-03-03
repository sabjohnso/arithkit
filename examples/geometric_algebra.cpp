/// @file geometric_algebra.cpp
/// @brief Reflections and rotors in Cl(3,0) using Clifford<double,3,0>.
///
/// Reflects a 3D vector through a plane using the Clifford sandwich
/// product v -> -n*v*n (where n is the unit normal), then composes
/// two reflections to produce a rotation (a rotor).
///
/// Geometric algebra unifies reflections, rotations, and projections
/// in a single algebraic framework — no matrices needed.

#include <cmath>
#include <iomanip>
#include <iostream>

#include <arithkit/clifford.hpp>

using arithkit::Clifford;

// Cl(3,0): e1^2 = e2^2 = e3^2 = +1
// Dimension = 2^3 = 8 basis elements:
//   index 0: scalar (1)
//   index 1: e1  (bitmask 001)
//   index 2: e2  (bitmask 010)
//   index 3: e1e2 (bitmask 011)
//   index 4: e3  (bitmask 100)
//   index 5: e1e3 (bitmask 101)
//   index 6: e2e3 (bitmask 110)
//   index 7: e1e2e3 (bitmask 111)

using GA = Clifford<double, 3, 0>;

auto
make_vector(double x, double y, double z) -> GA {
  GA v;
  v[1] = x; // e1
  v[2] = y; // e2
  v[4] = z; // e3
  return v;
}

void
print_vector(const char* label, const GA& v) {
  std::cout << std::setw(20) << label << " = (" << std::setw(8)
            << std::setprecision(4) << v[1] << ", " << std::setw(8) << v[2]
            << ", " << std::setw(8) << v[4] << ")\n";
}

int
main() {
  std::cout << "=== Geometric Algebra Cl(3,0) — Reflections & Rotors ===\n\n";

  // --- Reflection through a plane ---
  // Reflect v through the plane with unit normal n:
  //   v' = -n * v * n

  auto v = make_vector(1.0, 2.0, 3.0);
  auto n = make_vector(1.0, 0.0, 0.0); // YZ-plane normal

  // Reflection: negate the component along n
  auto v_ref = -n * v * n;

  std::cout << "Reflection through the YZ plane (normal = e1):\n";
  print_vector("original v", v);
  print_vector("reflected v", v_ref);
  std::cout << "Expected:          (-1, 2, 3) — x component negated\n\n";

  // --- Reflection through XZ plane ---
  auto n2 = make_vector(0.0, 1.0, 0.0); // XZ-plane normal
  auto v_ref2 = -n2 * v * n2;

  std::cout << "Reflection through the XZ plane (normal = e2):\n";
  print_vector("original v", v);
  print_vector("reflected v", v_ref2);
  std::cout << "Expected:          (1, -2, 3) — y component negated\n\n";

  // --- Rotation as composition of two reflections ---
  // Rotating by angle theta around an axis = reflecting through two
  // planes that meet at angle theta/2.
  //
  // The rotor R = n2 * n1 encodes this rotation.
  // v' = R * v * ~R  where ~R is the reverse of R.

  std::cout << "--- Rotation via two reflections ---\n\n";

  // Two normals separated by 45 degrees in the XY plane
  // -> rotation of 90 degrees around Z
  constexpr double pi = 3.14159265358979323846;
  double angle = pi / 4; // half the rotation angle
  auto n_a = make_vector(1.0, 0.0, 0.0);
  auto n_b = make_vector(std::cos(angle), std::sin(angle), 0.0);

  // Rotor = n_b * n_a
  auto rotor = n_b * n_a;

  // The reverse of a rotor R = n_b * n_a is ~R = n_a * n_b
  auto rotor_rev = n_a * n_b;

  auto w = make_vector(1.0, 0.0, 0.0);
  auto w_rot = rotor * w * rotor_rev;

  std::cout << "Rotor: two reflections at 45 degrees = 90-degree rotation around Z\n";
  print_vector("original", w);
  print_vector("rotated", w_rot);
  std::cout << "Expected:          (0, 1, 0)\n\n";

  // Apply rotor to (0,1,0)
  auto u = make_vector(0.0, 1.0, 0.0);
  auto u_rot = rotor * u * rotor_rev;
  print_vector("(0,1,0) rotated", u_rot);
  std::cout << "Expected:          (-1, 0, 0)\n\n";

  // --- Key insight ---
  std::cout
    << "Key insight: in geometric algebra, reflections and rotations\n"
    << "are both just algebraic products. No matrices, no trig\n"
    << "identities, no special cases — the algebra handles it all.\n";

  return 0;
}
