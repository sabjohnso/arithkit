#pragma once

/// @file monoid_augmentation_laws.hpp
/// @brief Structural laws for Sparsity<T> as a monoid augmentation.
///
/// Sparsity<T> augments scalar T with a dependency index set whose
/// accumulation follows (𝒫(ℕ), ∪, ∅). These laws capture two structural
/// invariants not covered by the generic arithmetic tests:
///
///   1. Embedding homomorphism: scalars (empty index set) compose like T.
///      E(a) op E(b) == E(a op b) for all four arithmetic operations.
///
///   2. Indicator-OR propagation: the "has indices" predicate is_aug satisfies
///      is_aug(a op b) == is_aug(a) || is_aug(b)
///      because (A ∪ B).empty() ⟺ A.empty() && B.empty().

#include <arithkit/sparsity.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include <algorithm>
#include <set>

namespace arithkit::laws {

  /// Check the embedding and indicator-OR laws for Sparsity<T>.
  ///
  /// Requires rc::Arbitrary<Sparsity<T>> to be defined by the calling
  /// translation unit. T must support +, -, *, /, default construction,
  /// and operator!=.
  template <typename T>
  void
  check_monoid_augmentation() {
    using E = Sparsity<T>;

    auto index_union = [](const E& a, const E& b) {
      std::set<std::size_t> result;
      std::set_union(
        a.indices().begin(),
        a.indices().end(),
        b.indices().begin(),
        b.indices().end(),
        std::inserter(result, result.begin()));
      return result;
    };

    auto is_aug = [](const E& x) { return !x.indices().empty(); };

    // --- Embedding homomorphism ---

    rc::prop("embedding +: E(a) + E(b) == E(a+b)", [](E a, E b) {
      auto sa = a.value();
      auto sb = b.value();
      RC_ASSERT(E(sa) + E(sb) == E(sa + sb));
    });

    rc::prop("embedding -: E(a) - E(b) == E(a-b)", [](E a, E b) {
      auto sa = a.value();
      auto sb = b.value();
      RC_ASSERT(E(sa) - E(sb) == E(sa - sb));
    });

    rc::prop("embedding *: E(a) * E(b) == E(a*b)", [](E a, E b) {
      auto sa = a.value();
      auto sb = b.value();
      RC_ASSERT(E(sa) * E(sb) == E(sa * sb));
    });

    rc::prop("embedding /: E(a) / E(b) == E(a/b)", [](E a, E b) {
      RC_PRE(b.value() != T{});
      auto sa = a.value();
      auto sb = b.value();
      RC_ASSERT(E(sa) / E(sb) == E(sa / sb));
    });

    // --- Indicator-OR propagation ---

    rc::prop(
      "indicator OR +: is_aug(a+b) == is_aug(a)||is_aug(b)",
      [&](E a, E b) { RC_ASSERT(is_aug(a + b) == (is_aug(a) || is_aug(b))); });

    rc::prop(
      "indicator OR -: is_aug(a-b) == is_aug(a)||is_aug(b)",
      [&](E a, E b) { RC_ASSERT(is_aug(a - b) == (is_aug(a) || is_aug(b))); });

    rc::prop(
      "indicator OR *: is_aug(a*b) == is_aug(a)||is_aug(b)",
      [&](E a, E b) { RC_ASSERT(is_aug(a * b) == (is_aug(a) || is_aug(b))); });

    rc::prop(
      "indicator OR /: is_aug(a/b) == is_aug(a)||is_aug(b)", [&](E a, E b) {
        RC_PRE(b.value() != T{});
        RC_ASSERT(is_aug(a / b) == (is_aug(a) || is_aug(b)));
      });

    // --- Division semantics (not covered by existing sparsity_test.cpp) ---

    rc::prop(
      "division index union: indices(a/b) == indices(a) ∪ indices(b)",
      [&](E a, E b) {
        RC_PRE(b.value() != T{});
        RC_ASSERT((a / b).indices() == index_union(a, b));
      });

    rc::prop("division value: value(a/b) == value(a)/value(b)", [](E a, E b) {
      RC_PRE(b.value() != T{});
      RC_ASSERT((a / b).value() == a.value() / b.value());
    });
  }

} // namespace arithkit::laws
