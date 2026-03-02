#pragma once

/// @file field_laws.hpp
/// @brief Property tests for Field (commutative division ring).

#include <arithkit/concepts/field.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck.h>
#include <rapidcheck/catch.h>

#include "commutative_ring_laws.hpp"
#include "division_ring_laws.hpp"

namespace arithkit::laws {

  template <Field T>
  void
  check_field() {
    check_commutative_ring<T>();
    check_division_ring<T>();
  }

} // namespace arithkit::laws
