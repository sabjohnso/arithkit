#include <arithkit/concepts.hpp>
#include <arithkit/traits/identity_element.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "laws/approx_ordered_field_laws.hpp"
#include "laws/euclidean_domain_laws.hpp"

using namespace arithkit;

// --- Static assertions: built-in types model the expected concepts ---

static_assert(AdditiveMagma<int>);
static_assert(AdditiveSemigroup<int>);
static_assert(AdditiveMonoid<int>);
static_assert(AdditiveGroup<int>);
static_assert(AdditiveAbelianGroup<int>);
static_assert(MultiplicativeMagma<int>);
static_assert(MultiplicativeSemigroup<int>);
static_assert(MultiplicativeMonoid<int>);
static_assert(Ring<int>);
static_assert(CommutativeRing<int>);
static_assert(IntegralDomain<int>);
static_assert(EuclideanDomain<int>);

static_assert(OrderedField<double>);
static_assert(Field<double>);

// --- Custom RapidCheck generator for small ints (avoids overflow) ---

namespace rc {

  template <>
  struct Arbitrary<int> {
    static Gen<int> arbitrary() {
      return gen::inRange(-1000, 1001);
    }
  };

} // namespace rc

// --- Identity element tests ---

TEST_CASE("identity_element for int", "[traits]") {
  CHECK(identity_v<int, additive_tag>() == 0);
  CHECK(identity_v<int, multiplicative_tag>() == 1);
}

TEST_CASE("identity_element for double", "[traits]") {
  CHECK(identity_v<double, additive_tag>() == 0.0);
  CHECK(identity_v<double, multiplicative_tag>() == 1.0);
}

// --- int models EuclideanDomain (all laws from the hierarchy) ---

TEST_CASE("int satisfies EuclideanDomain laws", "[laws][int]") {
  arithkit::laws::check_euclidean_domain<int>();
}

// --- double: approximate ordered field laws ---

TEST_CASE("double satisfies OrderedField laws (approximate)",
          "[laws][double]") {
  arithkit::laws::check_approx_ordered_field<double>();
}
