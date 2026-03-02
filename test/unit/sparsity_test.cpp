#include <arithkit/sparsity.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "laws/monoid_augmentation_laws.hpp"

#include <algorithm>
#include <set>

using namespace arithkit;

using Sd = Sparsity<double>;

// Sparsity<T> deliberately does NOT satisfy Ring.
// Additive inverse law fails for non-empty index sets:
//   (v,S) + (-v,S) = (0,S) ≠ (0,∅) when S is nonempty.
// No standard algebraic concept is claimed.

// --- RapidCheck generator ---

namespace rc {

  template <>
  struct Arbitrary<Sd> {
    static Gen<Sd> arbitrary() {
      return gen::exec([] {
        auto v = double(*gen::inRange(-50, 51));
        auto n = *gen::inRange(0, 4);
        std::set<std::size_t> idx;
        for (int i = 0; i < n; ++i)
          idx.insert(std::size_t(*gen::inRange(0, 8)));
        return Sd(v, idx);
      });
    }
  };

} // namespace rc

// --- Construction tests ---

TEST_CASE("Sparsity default constructs to (0, empty)", "[sparsity]") {
  Sd z;
  CHECK(z.value() == 0.0);
  CHECK(z.indices().empty());
}

TEST_CASE("Sparsity from scalar has empty index set", "[sparsity]") {
  Sd z(3.0);
  CHECK(z.value() == 3.0);
  CHECK(z.indices().empty());
}

TEST_CASE("Sparsity from scalar and initializer_list", "[sparsity]") {
  Sd z(2.0, {1, 3, 5});
  CHECK(z.value() == 2.0);
  CHECK(z.indices() == std::set<std::size_t>({1, 3, 5}));
}

TEST_CASE("Sparsity from scalar and set", "[sparsity]") {
  std::set<std::size_t> idx = {0, 2, 4};
  Sd z(7.0, idx);
  CHECK(z.value() == 7.0);
  CHECK(z.indices() == idx);
}

// --- Arithmetic: value and index-union semantics ---

TEST_CASE("Sparsity addition: scalar sum, union of indices", "[sparsity]") {
  Sd a(3.0, {0, 1});
  Sd b(4.0, {1, 2});
  auto c = a + b;
  CHECK(c.value() == 7.0);
  CHECK(c.indices() == std::set<std::size_t>({0, 1, 2}));
}

TEST_CASE("Sparsity subtraction: scalar diff, union of indices", "[sparsity]") {
  Sd a(10.0, {0, 2});
  Sd b(3.0, {1, 2});
  auto c = a - b;
  CHECK(c.value() == 7.0);
  CHECK(c.indices() == std::set<std::size_t>({0, 1, 2}));
}

TEST_CASE(
  "Sparsity multiplication: scalar product, union of indices", "[sparsity]") {
  Sd a(3.0, {0});
  Sd b(5.0, {1});
  auto c = a * b;
  CHECK(c.value() == 15.0);
  CHECK(c.indices() == std::set<std::size_t>({0, 1}));
}

TEST_CASE(
  "Sparsity division: scalar quotient, union of indices", "[sparsity]") {
  Sd a(6.0, {0, 1});
  Sd b(2.0, {2});
  auto c = a / b;
  CHECK(c.value() == 3.0);
  CHECK(c.indices() == std::set<std::size_t>({0, 1, 2}));
}

// --- Unary negation ---

TEST_CASE(
  "Sparsity unary negation negates value, keeps indices", "[sparsity]") {
  Sd a(5.0, {1, 3});
  auto b = -a;
  CHECK(b.value() == -5.0);
  CHECK(b.indices() == std::set<std::size_t>({1, 3}));
}

// --- Equality ---

TEST_CASE("Sparsity equality requires matching value AND indices", "[sparsity]") {
  Sd a(1.0, {0});
  Sd b(1.0, {0});
  Sd c(1.0, {1});
  Sd d(2.0, {0});
  CHECK(a == b);
  CHECK(!(a == c));
  CHECK(!(a == d));
}

// --- Self-subtraction: index set does not vanish ---

TEST_CASE("Sparsity self-subtraction preserves index set", "[sparsity]") {
  Sd x(5.0, {1, 2, 3});
  auto y = x - x;
  CHECK(y.value() == 0.0);
  // S ∪ S = S — index set is NOT emptied by cancellation of values
  CHECK(y.indices() == std::set<std::size_t>({1, 2, 3}));
}

// --- Compound assignment ---

TEST_CASE("Sparsity compound assignment +=", "[sparsity]") {
  Sd a(1.0, {0});
  Sd b(2.0, {1});
  a += b;
  CHECK(a.value() == 3.0);
  CHECK(a.indices() == std::set<std::size_t>({0, 1}));
}

TEST_CASE("Sparsity compound assignment -=", "[sparsity]") {
  Sd a(5.0, {0});
  Sd b(2.0, {1});
  a -= b;
  CHECK(a.value() == 3.0);
  CHECK(a.indices() == std::set<std::size_t>({0, 1}));
}

TEST_CASE("Sparsity compound assignment *=", "[sparsity]") {
  Sd a(3.0, {0});
  Sd b(4.0, {1});
  a *= b;
  CHECK(a.value() == 12.0);
  CHECK(a.indices() == std::set<std::size_t>({0, 1}));
}

TEST_CASE("Sparsity compound assignment /=", "[sparsity]") {
  Sd a(8.0, {0});
  Sd b(2.0, {1});
  a /= b;
  CHECK(a.value() == 4.0);
  CHECK(a.indices() == std::set<std::size_t>({0, 1}));
}

// --- Sparsity pattern extraction use case ---

TEST_CASE(
  "Sparsity pattern extraction from linear expression", "[sparsity]") {
  // Tag each variable with a unique index
  Sd x0(2.0, {0});
  Sd x1(3.0, {1});
  Sd x2(5.0, {2});

  // f = 2*x0 + 3*x1 depends on variables 0 and 1
  Sd two(2.0);
  Sd three(3.0);
  auto f = two * x0 + three * x1;
  CHECK(f.indices() == std::set<std::size_t>({0, 1}));

  // g = x1 * x2 depends on variables 1 and 2
  auto g = x1 * x2;
  CHECK(g.indices() == std::set<std::size_t>({1, 2}));

  // h = f + g depends on variables 0, 1, and 2
  auto h = f + g;
  CHECK(h.indices() == std::set<std::size_t>({0, 1, 2}));
}

// --- Property tests ---

TEST_CASE("Sparsity value semantics preserved under all ops", "[sparsity]") {
  rc::prop("value(a + b) == value(a) + value(b)", [](Sd a, Sd b) {
    RC_ASSERT((a + b).value() == a.value() + b.value());
  });
  rc::prop("value(a - b) == value(a) - value(b)", [](Sd a, Sd b) {
    RC_ASSERT((a - b).value() == a.value() - b.value());
  });
  rc::prop("value(a * b) == value(a) * value(b)", [](Sd a, Sd b) {
    RC_ASSERT((a * b).value() == a.value() * b.value());
  });
}

TEST_CASE("Sparsity indices are union under all ops", "[sparsity]") {
  auto index_union = [](const Sd& a, const Sd& b) {
    std::set<std::size_t> result;
    std::set_union(
      a.indices().begin(),
      a.indices().end(),
      b.indices().begin(),
      b.indices().end(),
      std::inserter(result, result.begin()));
    return result;
  };

  rc::prop("indices(a + b) == indices(a) ∪ indices(b)", [&](Sd a, Sd b) {
    RC_ASSERT((a + b).indices() == index_union(a, b));
  });
  rc::prop("indices(a - b) == indices(a) ∪ indices(b)", [&](Sd a, Sd b) {
    RC_ASSERT((a - b).indices() == index_union(a, b));
  });
  rc::prop("indices(a * b) == indices(a) ∪ indices(b)", [&](Sd a, Sd b) {
    RC_ASSERT((a * b).indices() == index_union(a, b));
  });
}

TEST_CASE("Sparsity index monotonicity under addition", "[sparsity]") {
  rc::prop("indices(a) ⊆ indices(a + b)", [](Sd a, Sd b) {
    auto combined = a + b;
    RC_ASSERT(std::includes(
      combined.indices().begin(),
      combined.indices().end(),
      a.indices().begin(),
      a.indices().end()));
    RC_ASSERT(std::includes(
      combined.indices().begin(),
      combined.indices().end(),
      b.indices().begin(),
      b.indices().end()));
  });
}

TEST_CASE(
  "Sparsity index set is commutative under addition", "[sparsity]") {
  rc::prop("indices(a + b) == indices(b + a)", [](Sd a, Sd b) {
    RC_ASSERT((a + b).indices() == (b + a).indices());
  });
}

TEST_CASE(
  "Sparsity<double> satisfies MonoidAugmentation laws",
  "[laws][sparsity]") {
  arithkit::laws::check_monoid_augmentation<double>();
}
