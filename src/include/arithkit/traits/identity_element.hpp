#pragma once

/// @file identity_element.hpp
/// @brief External trait for algebraic identity elements.
///
/// Provides identity_element<T, OpTag> — a customization point that
/// returns the identity element of type T under operation OpTag.
/// Specializable for any type, including built-ins.

#include <concepts>

namespace arithkit {

  /// Tag types for algebraic operations.
  struct additive_tag {};
  struct multiplicative_tag {};

  /// Primary template — not defined.  Users specialize for their types.
  /// Each specialization must provide a static `make()` factory function
  /// that returns the identity element.  This avoids static initialization
  /// order issues (SIOF) that arise with static data members.
  template <typename T, typename OpTag>
  struct identity_element;

  /// Convenience function returning the identity element.
  /// Delegates to identity_element<T, OpTag>::make() so each call
  /// constructs the value on demand — no static storage, no SIOF.
  template <typename T, typename OpTag>
  constexpr T
  identity_v() {
    return identity_element<T, OpTag>::make();
  }

  // --- Built-in specializations: additive identity (zero) ---

  template <std::integral T>
  struct identity_element<T, additive_tag> {
    static constexpr T
    make() {
      return T{0};
    }
  };

  template <std::floating_point T>
  struct identity_element<T, additive_tag> {
    static constexpr T
    make() {
      return T{0};
    }
  };

  // --- Built-in specializations: multiplicative identity (one) ---

  template <std::integral T>
  struct identity_element<T, multiplicative_tag> {
    static constexpr T
    make() {
      return T{1};
    }
  };

  template <std::floating_point T>
  struct identity_element<T, multiplicative_tag> {
    static constexpr T
    make() {
      return T{1};
    }
  };

} // namespace arithkit
