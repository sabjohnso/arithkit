/// @file root_isolation.cpp
/// @brief Verified root isolation using Interval<double>.
///
/// Uses interval bisection to find all roots of a polynomial in a
/// given range with guaranteed enclosures.  If 0 is not in f(X),
/// the subinterval provably contains no root.  If 0 is in f(X) and
/// the interval is narrow enough, it is a candidate root enclosure.
///
/// No root can be missed — the computation certifies its own
/// correctness.

#include <iomanip>
#include <iostream>
#include <vector>

#include <arithkit/interval.hpp>

using arithkit::Interval;
using I = Interval<double>;

/// Evaluate f(x) = x^3 - 6x^2 + 11x - 6 = (x-1)(x-2)(x-3)
/// generically over any arithmetic type.
template <typename T>
auto
f(T x) -> T {
  return x * x * x - T(6.0) * x * x + T(11.0) * x - T(6.0);
}

struct RootEnclosure {
  double lo, hi;
};

void
isolate_roots(
  I range,
  double tol,
  std::vector<RootEnclosure>& roots) {
  // Evaluate f on the interval
  auto frange = f(range);

  // If 0 is not in f(range), no root exists here
  if (frange.lo() > 0.0 || frange.hi() < 0.0) return;

  // If the interval is narrow enough, report it
  if (range.width() < tol) {
    roots.push_back({range.lo(), range.hi()});
    return;
  }

  // Bisect
  double mid = range.midpoint();
  isolate_roots(I(range.lo(), mid), tol, roots);
  isolate_roots(I(mid, range.hi()), tol, roots);
}

/// Merge adjacent/overlapping candidate intervals into single
/// root enclosures.  The bisection can produce many tiny touching
/// intervals near a root due to interval overestimation.
auto
merge_enclosures(const std::vector<RootEnclosure>& raw)
  -> std::vector<RootEnclosure> {
  if (raw.empty()) return {};
  std::vector<RootEnclosure> merged;
  merged.push_back(raw[0]);
  for (std::size_t i = 1; i < raw.size(); ++i) {
    if (raw[i].lo <= merged.back().hi) {
      merged.back().hi = raw[i].hi;
    } else {
      merged.push_back(raw[i]);
    }
  }
  return merged;
}

int
main() {
  std::cout << "=== Verified Root Isolation with Interval<double> ===\n\n";
  std::cout << "f(x) = x^3 - 6x^2 + 11x - 6 = (x-1)(x-2)(x-3)\n";
  std::cout << "Searching [0, 4] with tolerance 1e-10\n\n";

  I search_range(0.0, 4.0);
  double tol = 1e-10;
  std::vector<RootEnclosure> candidates;

  isolate_roots(search_range, tol, candidates);
  auto roots = merge_enclosures(candidates);

  std::cout << "Found " << roots.size() << " root enclosure(s):\n\n";
  for (std::size_t i = 0; i < roots.size(); ++i) {
    double mid = (roots[i].lo + roots[i].hi) / 2.0;
    double width = roots[i].hi - roots[i].lo;
    std::cout << "  Root " << (i + 1) << ": [" << std::setprecision(12)
              << roots[i].lo << ", " << roots[i].hi << "]\n"
              << "          midpoint = " << mid << "    width = " << std::scientific
              << width << std::fixed << "\n";
  }

  std::cout
    << "\nGuarantee: every true root in [0,4] is enclosed by some\n"
    << "interval above. No root can be missed — the interval\n"
    << "evaluation proves absence in discarded subintervals.\n";

  return 0;
}
