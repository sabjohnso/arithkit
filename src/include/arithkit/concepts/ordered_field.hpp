#pragma once

/// @file ordered_field.hpp
/// @brief OrderedField — a Field with a total order compatible with
///        field operations.
///
/// Laws (enforced by property tests):
///   total order:   a < b || a == b || a > b
///   compatibility: a < b  implies  a + c < b + c
///                  0 < a && 0 < b  implies  0 < a*b

#include <arithkit/concepts/field.hpp>

namespace arithkit {

  template <typename T>
  concept OrderedField = Field<T> && std::totally_ordered<T>;

} // namespace arithkit
