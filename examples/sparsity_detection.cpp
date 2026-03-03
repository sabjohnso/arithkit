/// @file sparsity_detection.cpp
/// @brief Jacobian sparsity pattern detection using Sparsity<double>.
///
/// Given a function f: R^3 -> R^2, automatically detects which inputs
/// affect which outputs by propagating Sparsity<double> through the
/// computation.  The index sets at the output reveal the Jacobian's
/// sparsity pattern without computing any actual derivatives.

#include <iomanip>
#include <iostream>

#include <arithkit/sparsity.hpp>

using arithkit::Sparsity;
using S = Sparsity<double>;

/// A function f: R^3 -> R^2 with known sparsity structure:
///   f1(x0, x1, x2) = x0 * x1 + 3.0      (depends on x0, x1)
///   f2(x0, x1, x2) = x1 * x2 - x0        (depends on x0, x1, x2)
auto
compute_f(S x0, S x1, S x2) -> std::pair<S, S> {
  auto f1 = x0 * x1 + 3.0;
  auto f2 = x1 * x2 - x0;
  return {f1, f2};
}

void
print_indices(const char* label, const S& s) {
  std::cout << std::setw(8) << label << " depends on: {";
  bool first = true;
  for (auto idx : s.indices()) {
    if (!first) std::cout << ", ";
    std::cout << "x" << idx;
    first = false;
  }
  std::cout << "}    value = " << s.value() << "\n";
}

int
main() {
  std::cout << "=== Jacobian Sparsity Detection with Sparsity<double> ===\n\n";

  std::cout << "Function f: R^3 -> R^2:\n";
  std::cout << "  f1(x0,x1,x2) = x0*x1 + 3\n";
  std::cout << "  f2(x0,x1,x2) = x1*x2 - x0\n\n";

  // Tag each input with its index
  S x0(1.0, {0}); // variable 0
  S x1(2.0, {1}); // variable 1
  S x2(3.0, {2}); // variable 2

  auto [f1, f2] = compute_f(x0, x1, x2);

  std::cout << "Sparsity analysis (at x = [1, 2, 3]):\n";
  print_indices("f1", f1);
  print_indices("f2", f2);

  std::cout << "\nJacobian sparsity pattern:\n\n";
  std::cout << "         x0  x1  x2\n";
  std::cout << "  f1   [";
  std::cout << (f1.indices().count(0) ? " * " : " . ");
  std::cout << (f1.indices().count(1) ? " * " : " . ");
  std::cout << (f1.indices().count(2) ? " * " : " . ");
  std::cout << "]\n";
  std::cout << "  f2   [";
  std::cout << (f2.indices().count(0) ? " * " : " . ");
  std::cout << (f2.indices().count(1) ? " * " : " . ");
  std::cout << (f2.indices().count(2) ? " * " : " . ");
  std::cout << "]\n\n";

  // A more complex example: chain of operations
  std::cout << "--- More complex example ---\n\n";
  std::cout << "g: R^4 -> R^3:\n";
  std::cout << "  g1 = x0 + x1\n";
  std::cout << "  g2 = x2 * x3\n";
  std::cout << "  g3 = x0 * x2 + x1 * x3\n\n";

  S y0(1.0, {0});
  S y1(2.0, {1});
  S y2(3.0, {2});
  S y3(4.0, {3});

  auto g1 = y0 + y1;
  auto g2 = y2 * y3;
  auto g3 = y0 * y2 + y1 * y3;

  print_indices("g1", g1);
  print_indices("g2", g2);
  print_indices("g3", g3);

  std::cout << "\nJacobian sparsity pattern:\n\n";
  std::cout << "         x0  x1  x2  x3\n";
  auto print_row = [](const char* label, const S& gi) {
    std::cout << label;
    for (std::size_t j = 0; j < 4; ++j) {
      std::cout << (gi.indices().count(j) ? " * " : " . ");
    }
    std::cout << "]\n";
  };
  print_row("  g1   [", g1);
  print_row("  g2   [", g2);
  print_row("  g3   [", g3);

  std::cout << "\nThe sparsity pattern reveals which derivatives are\n"
            << "structurally nonzero — enabling efficient Jacobian\n"
            << "computation via graph coloring or sparse AD.\n";

  return 0;
}
