/// @file mandelbrot.cpp
/// @brief ASCII-art Mandelbrot set using Complex<double>.
///
/// Iterates z <- z^2 + c for a grid of complex points.  The escape
/// time determines the character displayed.  A classic exercise in
/// complex multiplication and norm computation.

#include <iostream>

#include <arithkit/complex.hpp>

using arithkit::Complex;
using C = Complex<double>;

int
main() {
  constexpr int width = 78;
  constexpr int height = 36;
  constexpr int max_iter = 80;

  constexpr double x_min = -2.2, x_max = 0.8;
  constexpr double y_min = -1.2, y_max = 1.2;

  constexpr char palette[] = " .:-=+*#%@";
  constexpr int palette_size = sizeof(palette) - 1;

  std::cout << "=== Mandelbrot Set with Complex<double> ===\n\n";

  for (int row = 0; row < height; ++row) {
    double im =
      y_max - row * (y_max - y_min) / (height - 1);
    for (int col = 0; col < width; ++col) {
      double re =
        x_min + col * (x_max - x_min) / (width - 1);

      C c(re, im);
      C z;

      int iter = 0;
      while (iter < max_iter && norm(z) < 4.0) {
        z = z * z + c;
        ++iter;
      }

      if (iter == max_iter) {
        std::cout << palette[palette_size - 1];
      } else {
        std::cout << palette[iter % palette_size];
      }
    }
    std::cout << "\n";
  }

  std::cout
    << "\nPoints that never escape ('" << palette[palette_size - 1]
    << "') are in the Mandelbrot set.\n"
    << "The boundary shows the fractal structure of z -> z^2 + c.\n";

  return 0;
}
