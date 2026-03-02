#include <arithkit/affine.hpp>
#include <arithkit/concepts.hpp>
#include <arithkit/interval.hpp>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

using Catch::Approx;
using namespace arithkit;

using Ad = Affine<double>;
using Ivd = Interval<double>;

// --- Static concept checks ---

static_assert(IntervalArithmetic<Ad>);

// --- RapidCheck generators ---

namespace rc {

  template <>
  struct Arbitrary<Ad> {
    static Gen<Ad> arbitrary() {
      return gen::exec([] {
        auto center = double(*gen::inRange(-100, 101));
        auto n = *gen::inRange(0, 4);
        std::vector<double> noise;
        noise.reserve(n);
        for (int i = 0; i < n; ++i) {
          noise.push_back(double(*gen::inRange(-50, 51)));
        }
        return Ad(center, std::move(noise));
      });
    }
  };

} // namespace rc

// --- Construction tests ---

TEST_CASE("Affine default constructs to zero", "[affine]") {
  Ad z;
  CHECK(z.center() == 0.0);
  CHECK(z.to_interval() == Ivd(0.0, 0.0));
}

TEST_CASE("Affine from single value", "[affine]") {
  Ad z(3.0);
  CHECK(z.center() == 3.0);
  CHECK(z.to_interval() == Ivd(3.0, 3.0));
}

TEST_CASE("Affine from center and noise terms", "[affine]") {
  // 5 + 2ε₁ + 3ε₂ => range is [5-5, 5+5] = [0, 10]
  Ad z(5.0, {2.0, 3.0});
  CHECK(z.center() == 5.0);
  CHECK(z.to_interval() == Ivd(0.0, 10.0));
}

// --- Interval conversion ---

TEST_CASE("Affine to_interval computes correct bounds", "[affine]") {
  // 10 + 3ε₁ - 2ε₂ => radius = |3| + |-2| = 5 => [5, 15]
  Ad z(10.0, {3.0, -2.0});
  auto iv = z.to_interval();
  CHECK(iv.lo() == Approx(5.0));
  CHECK(iv.hi() == Approx(15.0));
}

// --- Basic arithmetic ---

TEST_CASE("Affine addition", "[affine]") {
  // (3 + 2ε₁) + (5 + 1ε₁) = 8 + 3ε₁
  Ad a(3.0, {2.0});
  Ad b(5.0, {1.0});
  auto c = a + b;
  CHECK(c.center() == 8.0);
  CHECK(c.to_interval() == Ivd(5.0, 11.0));
}

TEST_CASE("Affine subtraction", "[affine]") {
  // (3 + 2ε₁) - (5 + 1ε₁) = -2 + 1ε₁
  Ad a(3.0, {2.0});
  Ad b(5.0, {1.0});
  auto c = a - b;
  CHECK(c.center() == -2.0);
  CHECK(c.to_interval() == Ivd(-3.0, -1.0));
}

TEST_CASE("Affine scalar multiplication", "[affine]") {
  // (3 + 2ε₁) * (4) = 12 + 8ε₁
  Ad a(3.0, {2.0});
  Ad b(4.0);
  auto c = a * b;
  CHECK(c.center() == 12.0);
  CHECK(c.to_interval() == Ivd(4.0, 20.0));
}

// --- Correlation tracking ---

TEST_CASE("Affine tracks correlations (tighter than interval)",
          "[affine]") {
  // x = 5 + 3ε₁  => x ∈ [2, 8]
  // y = x - x should be exactly 0 (via correlated noise cancellation)
  Ad x(5.0, {3.0});
  auto y = x - x;
  CHECK(y.center() == 0.0);
  CHECK(y.to_interval() == Ivd(0.0, 0.0));

  // With plain intervals: [2,8] - [2,8] = [-6, 6]  (much wider!)
}

// --- Negation ---

TEST_CASE("Affine negation", "[affine]") {
  Ad z(3.0, {2.0, -1.0});
  auto neg = -z;
  CHECK(neg.center() == -3.0);
  CHECK(neg.to_interval() == Ivd(-6.0, 0.0));
}

// --- Division ---

TEST_CASE("Affine division by constant", "[affine]") {
  Ad a(6.0, {4.0});
  Ad b(2.0);
  auto c = a / b;
  CHECK(c.center() == Approx(3.0));
  CHECK(c.to_interval().lo() == Approx(1.0));
  CHECK(c.to_interval().hi() == Approx(5.0));
}

// --- Property tests: containment ---

TEST_CASE("Affine addition containment", "[laws][affine]") {
  rc::prop("affine addition contains midpoint sum", [](Ad X, Ad Y) {
    auto x = X.center();
    auto y = Y.center();
    auto result = X + Y;
    RC_ASSERT(result.to_interval().contains(x + y));
  });
}

TEST_CASE("Affine subtraction containment", "[laws][affine]") {
  rc::prop(
    "affine subtraction contains midpoint diff", [](Ad X, Ad Y) {
      auto x = X.center();
      auto y = Y.center();
      auto result = X - Y;
      RC_ASSERT(result.to_interval().contains(x - y));
    });
}

TEST_CASE("Affine multiplication containment", "[laws][affine]") {
  rc::prop(
    "affine multiplication contains midpoint product", [](Ad X, Ad Y) {
      auto x = X.center();
      auto y = Y.center();
      auto result = X * Y;
      RC_ASSERT(result.to_interval().contains(x * y));
    });
}

TEST_CASE("Affine bounds are tighter than or equal to interval",
          "[laws][affine]") {
  rc::prop(
    "affine interval subset of naive interval", [](Ad X, Ad Y) {
      auto xiv = X.to_interval();
      auto yiv = Y.to_interval();
      auto affine_result = (X + Y).to_interval();
      auto interval_result = xiv + yiv;
      // Affine result should be subset (or equal) to interval result
      RC_ASSERT(affine_result.is_subset_of(interval_result));
    });
}
