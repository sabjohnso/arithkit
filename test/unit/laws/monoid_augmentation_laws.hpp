#pragma once

/// @file monoid_augmentation_laws.hpp
/// @brief Structural laws for Sparsity<T> as a monoid augmentation.
///
/// Sparsity<T> augments scalar T with a dependency index set whose
/// accumulation follows (𝒫(ℕ), ∪, ∅). The monoid augmentation law suite
/// builds on the full augmentation hierarchy:
///   MagmaAugmentation → CommutativeMagmaAugmentation
///     → CommutativeMagmaWithIdentityAugmentation → MonoidAugmentation
///
/// This top-level suite adds the monoid-specific laws:
///   1. Embedding homomorphism: E(a) op E(b) == E(a op b)
///   2. Indicator-OR propagation: is_aug(a op b) == is_aug(a) || is_aug(b)
///   3. Index union: indices(a op b) == indices(a) ∪ indices(b)
///   4. Associativity of index combining

#include <arithkit/sparsity.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "commutative_magma_with_identity_augmentation_laws.hpp"

#include <algorithm>
#include <set>

namespace arithkit::laws {

  /// Check the full monoid augmentation law suite for Sparsity<T>.
  ///
  /// Calls the complete augmentation hierarchy chain, then adds
  /// monoid-specific laws: embedding homomorphism, indicator-OR,
  /// index union, and associativity.
  ///
  /// Requires rc::Arbitrary<Sparsity<T>> to be defined by the calling
  /// translation unit. T must support +, -, *, /, default construction,
  /// and operator!=.
  template <typename T>
  void
  check_monoid_augmentation() {
    using E = Sparsity<T>;

    // --- Full augmentation hierarchy ---
    check_commutative_magma_with_identity_augmentation<T, E>();

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

    // --- Index union ---

    rc::prop(
      "index union +: indices(a+b) == indices(a) ∪ indices(b)",
      [&](E a, E b) { RC_ASSERT((a + b).indices() == index_union(a, b)); });

    rc::prop(
      "index union -: indices(a-b) == indices(a) ∪ indices(b)",
      [&](E a, E b) { RC_ASSERT((a - b).indices() == index_union(a, b)); });

    rc::prop(
      "index union *: indices(a*b) == indices(a) ∪ indices(b)",
      [&](E a, E b) { RC_ASSERT((a * b).indices() == index_union(a, b)); });

    rc::prop(
      "index union /: indices(a/b) == indices(a) ∪ indices(b)", [&](E a, E b) {
        RC_PRE(b.value() != T{});
        RC_ASSERT((a / b).indices() == index_union(a, b));
      });

    // --- Associativity of index combining ---

    rc::prop(
      "assoc +: indices((a+b)+c) == indices(a+(b+c))", [](E a, E b, E c) {
        RC_ASSERT(((a + b) + c).indices() == (a + (b + c)).indices());
      });

    rc::prop(
      "assoc *: indices((a*b)*c) == indices(a*(b*c))", [](E a, E b, E c) {
        RC_ASSERT(((a * b) * c).indices() == (a * (b * c)).indices());
      });
  }

} // namespace arithkit::laws
