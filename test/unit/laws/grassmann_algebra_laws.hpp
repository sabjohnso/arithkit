#pragma once

/// @file grassmann_algebra_laws.hpp
/// @brief Grassmann (exterior) algebra structural laws: nilpotency and
/// anticommutativity.
///
/// In the exterior algebra ∧(Tⁿ), generators satisfy eᵢ∧eᵢ = 0
/// (nilpotency) and eᵢ∧eⱼ = −eⱼ∧eᵢ for i ≠ j (anticommutativity).

#include <arithkit/grassmann.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

namespace arithkit::laws {

  template <typename T, std::size_t N>
  void
  check_grassmann_algebra() {
    using G = Grassmann<T, N>;

    auto basis = [](std::size_t idx) {
      G e;
      e[std::size_t(1) << idx] = T(1);
      return e;
    };

    rc::prop("nilpotency: ei^ei = 0", [&basis]() {
      auto i = *rc::gen::inRange<std::size_t>(0, N);
      auto ei = basis(i);
      auto zero = G();
      RC_ASSERT(ei * ei == zero);
    });

    if constexpr (N >= 2) {
      rc::prop("anticommutativity: ei*ej = -ej*ei for i != j", [&basis]() {
        auto i = *rc::gen::inRange<std::size_t>(0, N);
        auto offset = *rc::gen::inRange<std::size_t>(1, N);
        auto j = (i + offset) % N;
        auto ei = basis(i);
        auto ej = basis(j);
        RC_ASSERT(ei * ej == -(ej * ei));
      });
    }
  }

} // namespace arithkit::laws
