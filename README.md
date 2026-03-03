# ArithKit

A C++20 header-only library of arithmetic types for numerical analysis.
Each type is designed as an algebra: the algebraic laws it satisfies are
stated as C++20 concepts and verified by property-based tests.

**Version:** 0.1.0

---

## Types

| Type | Header | Models | Notes |
|------|--------|--------|-------|
| `BigInt` | `arithkit/bigint.hpp` | `EuclideanDomain` | Arbitrary-precision integer; base-2³² limbs, schoolbook algorithms |
| `Fraction<I>` | `arithkit/fraction.hpp` | `Field` (or `OrderedField` if `I` is ordered) | Exact rational; auto-reduces via GCD |
| `ArbitraryFloat` | `arithkit/arbitrary_float.hpp` | `OrderedField` (approximate) | BigInt mantissa × 10^exp |
| `Complex<T>` | `arithkit/complex.hpp` | `Field` when `T:Field`, else `Ring` | a + bi, i²=−1; no total order |
| `Dual<T>` | `arithkit/dual.hpp` | `CommutativeRing` | a + bε, ε²=0; forward-mode automatic differentiation |
| `Interval<T>` | `arithkit/interval.hpp` | `IntervalArithmetic` | Closed interval [lo, hi]; subdistributive, not a ring |
| `Affine<T>` | `arithkit/affine.hpp` | `IntervalArithmetic` | Noise-symbol form; tighter bounds than plain intervals |
| `Simd<T,N>` | `arithkit/simd.hpp` | `CommutativeRing` | N-wide component-wise arithmetic; product ring, not a field |
| `Sparsity<T>` | `arithkit/sparsity.hpp` | — | Scalar + dependency index set; propagates sparsity patterns |
| `Quaternion<T>` | `arithkit/quaternion.hpp` | `DivisionRing` | w + xi + yj + zk; non-commutative multiplication |
| `Clifford<T,p,q>` | `arithkit/clifford.hpp` | `Ring` | Cl(p,q); 2^(p+q) dimensions; Cl(0,1)≅Complex, Cl(0,2)≅Quaternions |
| `Grassmann<T,n>` | `arithkit/grassmann.hpp` | `Ring` | Exterior algebra ∧(Tⁿ); 2^n dimensions; eᵢ∧eᵢ=0, eᵢ∧eⱼ=−eⱼ∧eᵢ |

---

## Concept Hierarchy

```
AdditiveMagma → AdditiveSemigroup → AdditiveMonoid → AdditiveGroup → AdditiveAbelianGroup
                                                                             │
MultiplicativeMonoid ────────────────────────────────────────────────────────┤
                                                                             ↓
                                                                           Ring
                                                                             │
                                                         ┌───────────────────┤
                                                         │                   │
                                                  CommutativeRing       DivisionRing
                                                         │
                                                  IntegralDomain
                                                         │
                                                  EuclideanDomain
                                                         │
                                                       Field
                                                         │
                                                   OrderedField
```

All concepts are in `arithkit/concepts/`. The umbrella header
`arithkit/concepts.hpp` includes the full hierarchy.

---

## Design

**Algebra-driven.** Every type declares the algebraic laws it satisfies as
a C++20 concept. Template parameters are constrained by concept, so
mismatches produce clear error messages at instantiation.

**Header-only.** Include a single header per type; no link step required.
The CMake target `arithkit::header` is an `INTERFACE` library.

**`constexpr` throughout.** All constructors and non-mutating operators on
stack-based types (`Complex`, `Dual`, `Quaternion`, `Interval`, `Simd`,
`Clifford`, `Grassmann`) are `constexpr`. Values can be computed and
verified entirely at compile time.

**Identity elements without SIOF.** The `identity_element<T, Tag>` trait
provides additive and multiplicative identities via a `static constexpr make()`
factory — never a static data member — avoiding static-initialization-order
issues across translation units.

**Property-based tests.** Algebraic laws (commutativity, associativity,
distributivity, inverse laws, etc.) are verified with
[RapidCheck](https://github.com/emil-e/rapidcheck) property tests, not
hand-picked examples.

---

## Building

Requires CMake ≥ 3.21 and a C++20 compiler. Dependencies (Catch2,
RapidCheck) are fetched automatically as CMake subprojects.

```bash
# Configure and build
cmake --preset default
cmake --build --preset default

# Run all tests
ctest --preset default

# Build and test with AddressSanitizer + UBSanitizer
cmake --preset sanitary
cmake --build --preset sanitary
ctest --preset sanitary

# Format all source files (clang-format)
./scripts/format.sh

# Check formatting compliance (prints count of non-compliant files)
./scripts/check-format.sh
```

Build output goes to `build/` (default preset) or `build-multi/` (Ninja
multi-config preset).

---

## Using ArithKit in Your Project

```cmake
find_package(arithkit REQUIRED)
target_link_libraries(my_target PRIVATE arithkit::header)
```

Then include whichever headers you need:

```cpp
#include <arithkit/complex.hpp>
#include <arithkit/dual.hpp>
#include <arithkit/concepts.hpp>   // full concept hierarchy

// constexpr arithmetic
constexpr auto z = arithkit::Complex<int>(3, 4) * arithkit::Complex<int>(1, -2);
static_assert(z == arithkit::Complex<int>(11, -2));

// forward-mode automatic differentiation
arithkit::Dual<double> x(3.0, 1.0); // x = 3 + 1ε
auto fx = x * x * x;                // f(x) = x³
// fx.real() == 27.0, fx.dual() == 27.0 (f'(x) = 3x² = 27 at x=3)
```

---

## Requirements

- **C++ standard:** C++20
- **Compiler flags:** `-Wall -Wextra -pedantic -Werror`
- **Formatting:** clang-format; 2-space indent, 80-column limit, K&R braces
