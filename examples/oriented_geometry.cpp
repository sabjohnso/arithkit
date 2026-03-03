/// @file oriented_geometry.cpp
/// @brief Oriented area and volume using Grassmann<double,3>.
///
/// The wedge product directly encodes oriented area and volume.
/// e1 ^ e2 gives the signed area of a parallelogram; a triple wedge
/// gives signed volume.  Sign flips indicate orientation reversal —
/// a direct encoding of handedness.

#include <cmath>
#include <iomanip>
#include <iostream>

#include <arithkit/grassmann.hpp>

using arithkit::Grassmann;

// Grassmann<double, 3>: exterior algebra over R^3
// 2^3 = 8 basis elements:
//   index 0: scalar (1)
//   index 1: e1  (bitmask 001)
//   index 2: e2  (bitmask 010)
//   index 3: e1^e2 (bitmask 011)
//   index 4: e3  (bitmask 100)
//   index 5: e1^e3 (bitmask 101)
//   index 6: e2^e3 (bitmask 110)
//   index 7: e1^e2^e3 (bitmask 111)

using G = Grassmann<double, 3>;

auto
make_vec(double x, double y, double z) -> G {
  G v;
  v[1] = x; // e1
  v[2] = y; // e2
  v[4] = z; // e3
  return v;
}

auto
bivector_magnitude(const G& bv) -> double {
  // A bivector has components in e1^e2, e1^e3, e2^e3
  return std::sqrt(bv[3] * bv[3] + bv[5] * bv[5] + bv[6] * bv[6]);
}

int
main() {
  std::cout << "=== Oriented Geometry with Grassmann<double,3> ===\n\n";
  std::cout << std::setprecision(4);

  // --- Triangle area ---
  // Triangle with vertices A=(0,0,0), B=(3,0,0), C=(0,4,0)
  // Edge vectors: u = B-A, v = C-A
  // Area = |u ^ v| / 2
  auto u = make_vec(3.0, 0.0, 0.0);
  auto v = make_vec(0.0, 4.0, 0.0);
  auto uv = u * v; // wedge product

  std::cout << "--- Triangle Area ---\n";
  std::cout << "Vertices: A=(0,0,0), B=(3,0,0), C=(0,4,0)\n";
  std::cout << "u = B-A = (3,0,0),  v = C-A = (0,4,0)\n";
  std::cout << "u ^ v = " << uv[3] << " e1^e2 + " << uv[5] << " e1^e3 + "
            << uv[6] << " e2^e3\n";
  std::cout << "Triangle area = |u^v|/2 = " << bivector_magnitude(uv) / 2.0
            << "\n";
  std::cout << "Expected: 6.0  (3*4/2 right triangle)\n\n";

  // --- Orientation reversal ---
  auto vu = v * u; // reversed order
  std::cout << "--- Orientation ---\n";
  std::cout << "u ^ v = " << uv[3] << " e1^e2\n";
  std::cout << "v ^ u = " << vu[3] << " e1^e2  (sign flipped!)\n";
  std::cout << "Reversing the order negates the signed area.\n\n";

  // --- Tetrahedron volume ---
  // Tetrahedron with vertices A=(0,0,0), B=(1,0,0), C=(0,1,0), D=(0,0,1)
  auto a = make_vec(1.0, 0.0, 0.0);
  auto b = make_vec(0.0, 1.0, 0.0);
  auto c = make_vec(0.0, 0.0, 1.0);

  auto abc = a * b * c; // triple wedge product
  double volume = abc[7] / 6.0; // e1^e2^e3 coefficient / 6

  std::cout << "--- Tetrahedron Volume ---\n";
  std::cout << "Vertices: O=(0,0,0), A=(1,0,0), B=(0,1,0), C=(0,0,1)\n";
  std::cout << "a^b^c = " << abc[7] << " e1^e2^e3\n";
  std::cout << "Volume = (a^b^c) / 6 = " << volume << "\n";
  std::cout << "Expected: 1/6 ≈ 0.1667  (unit simplex)\n\n";

  // --- Volume orientation ---
  auto cba = c * b * a;
  std::cout << "--- Volume Orientation ---\n";
  std::cout << "a^b^c = " << abc[7] << " e1^e2^e3  (right-handed)\n";
  std::cout << "c^b^a = " << cba[7] << " e1^e2^e3  (left-handed)\n";
  std::cout
    << "Reversing the order flips the sign — orientation is intrinsic.\n\n";

  // --- Parallelogram in 3D ---
  auto p = make_vec(1.0, 1.0, 0.0);
  auto q = make_vec(0.0, 1.0, 1.0);
  auto pq = p * q;

  std::cout << "--- Parallelogram in 3D ---\n";
  std::cout << "p = (1,1,0),  q = (0,1,1)\n";
  std::cout << "p^q = " << pq[3] << " e1^e2 + " << pq[5] << " e1^e3 + "
            << pq[6] << " e2^e3\n";
  std::cout << "Area = |p^q| = " << bivector_magnitude(pq) << "\n";
  std::cout << "The bivector components are the normal vector (1,-1,1),\n"
            << "encoding both area and orientation of the surface.\n";

  return 0;
}
