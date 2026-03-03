#pragma once

/// @file clifford_algebra_laws.hpp
/// @brief Clifford algebra structural laws: basis squares and
/// anticommutativity.
///
/// In Cl(p,q), the first p generators square to +1 and the next q square to −1.
/// Distinct generators anticommute: eᵢeⱼ = −eⱼeᵢ for i ≠ j.

#include <arithkit/clifford.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

namespace arithkit::laws {

  template <typename T, std::size_t P, std::size_t Q>
  void
  check_clifford_algebra() {
    using Cl = Clifford<T, P, Q>;
    constexpr auto N = P + Q;

    auto basis = [](std::size_t idx) {
      Cl e;
      e[std::size_t(1) << idx] = T(1);
      return e;
    };

    rc::prop(
      "basis squares: ei^2 = +1 for i<P, ei^2 = -1 for P<=i<P+Q", [&basis]() {
        auto i = *rc::gen::inRange<std::size_t>(0, N);
        auto ei = basis(i);
        auto expected = Cl(i < P ? T(1) : T(-1));
        RC_ASSERT(ei * ei == expected);
      });

    if constexpr (N >= 2) {
      rc::prop(
        "basis anticommutativity: ei*ej = -ej*ei for i != j", [&basis]() {
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
