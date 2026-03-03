#pragma once

/// @file concepts.hpp
/// @brief Umbrella header for all ArithKit algebraic concepts.

// Additive hierarchy
#include <arithkit/concepts/additive_abelian_group.hpp>
#include <arithkit/concepts/additive_group.hpp>
#include <arithkit/concepts/additive_magma.hpp>
#include <arithkit/concepts/additive_monoid.hpp>
#include <arithkit/concepts/additive_semigroup.hpp>

// Multiplicative hierarchy
#include <arithkit/concepts/multiplicative_magma.hpp>
#include <arithkit/concepts/multiplicative_monoid.hpp>
#include <arithkit/concepts/multiplicative_semigroup.hpp>
#include <arithkit/concepts/multiplicative_unital_magma.hpp>

// Ring hierarchy
#include <arithkit/concepts/alternative_division_algebra.hpp>
#include <arithkit/concepts/commutative_ring.hpp>
#include <arithkit/concepts/division_ring.hpp>
#include <arithkit/concepts/euclidean_domain.hpp>
#include <arithkit/concepts/field.hpp>
#include <arithkit/concepts/integral_domain.hpp>
#include <arithkit/concepts/non_associative_ring.hpp>
#include <arithkit/concepts/ordered_field.hpp>
#include <arithkit/concepts/ring.hpp>

// Two-sorted
#include <arithkit/concepts/algebra.hpp>
#include <arithkit/concepts/module.hpp>

// Augmentation
#include <arithkit/concepts/linear_augmentation.hpp>
#include <arithkit/concepts/monoid_augmentation.hpp>

// Auxiliary
#include <arithkit/concepts/interval_arithmetic.hpp>
#include <arithkit/concepts/normed.hpp>
