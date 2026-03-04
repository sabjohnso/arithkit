#pragma once

/// @file matrix.hpp
/// @brief Fixed-size matrix with compile-time dimensions.
///
/// Matrix<T, Rows, Cols> stores Rows×Cols elements of type T in
/// row-major order.  All matrices form an AdditiveAbelianGroup
/// (component-wise +/−).  Square matrices form a Ring under matrix
/// multiplication.
///
/// NOT CommutativeRing for N≥2 (semantically — multiplication is
/// non-commutative).  NOT DivisionRing (singular matrices exist).

#include <array>
#include <cstddef>
#include <ostream>
#include <utility>

#include <arithkit/concepts/commutative_ring.hpp>
#include <arithkit/concepts/field.hpp>
#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  namespace detail {

    /// Remove row `skip_row` and column `skip_col` from an N×N matrix,
    /// producing an (N-1)×(N-1) matrix.
    template <typename Mat, typename SmallMat>
    constexpr SmallMat
    submatrix(const Mat& m, std::size_t skip_row, std::size_t skip_col) {
      constexpr auto N = Mat::rows;
      constexpr auto M = N - 1;
      using T = typename Mat::value_type;
      std::array<T, M * M> data;
      std::size_t idx = 0;
      for (std::size_t r = 0; r < N; ++r) {
        if (r == skip_row) continue;
        for (std::size_t c = 0; c < N; ++c) {
          if (c == skip_col) continue;
          data[idx++] = m(r, c);
        }
      }
      return SmallMat(data);
    }

  } // namespace detail

  template <CommutativeRing T, std::size_t Rows, std::size_t Cols>
    requires(Rows > 0 && Cols > 0)
  class Matrix {
  public:
    using value_type = T;
    static constexpr std::size_t rows = Rows;
    static constexpr std::size_t cols = Cols;

    // --- Construction ---

    constexpr Matrix() {
      auto zero = identity_v<T, additive_tag>();
      data_.fill(zero);
    }

    constexpr explicit Matrix(std::array<T, Rows * Cols> data)
        : data_(std::move(data)) {}

    /// Scalar constructor (square only): builds scalar × identity.
    constexpr explicit Matrix(const T& scalar)
      requires(Rows == Cols)
    {
      auto zero = identity_v<T, additive_tag>();
      data_.fill(zero);
      for (std::size_t i = 0; i < Rows; ++i) {
        data_[i * Cols + i] = scalar;
      }
    }

    // --- Accessors ---

    [[nodiscard]] constexpr const T&
    operator()(std::size_t row, std::size_t col) const {
      return data_[row * Cols + col];
    }

    [[nodiscard]] constexpr T&
    operator()(std::size_t row, std::size_t col) {
      return data_[row * Cols + col];
    }

    // --- Comparison ---

    friend constexpr bool
    operator==(const Matrix& a, const Matrix& b) {
      return a.data_ == b.data_;
    }

    // --- Unary ---

    constexpr Matrix
    operator-() const {
      Matrix result;
      for (std::size_t i = 0; i < Rows * Cols; ++i) {
        result.data_[i] = -data_[i];
      }
      return result;
    }

    constexpr Matrix
    operator+() const {
      return *this;
    }

    // --- Component-wise addition ---

    friend constexpr Matrix
    operator+(const Matrix& a, const Matrix& b) {
      Matrix result;
      for (std::size_t i = 0; i < Rows * Cols; ++i) {
        result.data_[i] = a.data_[i] + b.data_[i];
      }
      return result;
    }

    // --- Component-wise subtraction ---

    friend constexpr Matrix
    operator-(const Matrix& a, const Matrix& b) {
      Matrix result;
      for (std::size_t i = 0; i < Rows * Cols; ++i) {
        result.data_[i] = a.data_[i] - b.data_[i];
      }
      return result;
    }

    // --- Matrix multiplication (ring operator, square only) ---

    friend constexpr Matrix
    operator*(const Matrix& a, const Matrix& b)
      requires(Rows == Cols)
    {
      auto zero = identity_v<T, additive_tag>();
      Matrix result;
      for (std::size_t r = 0; r < Rows; ++r) {
        for (std::size_t c = 0; c < Cols; ++c) {
          T sum = zero;
          for (std::size_t k = 0; k < Cols; ++k) {
            sum = sum + a.data_[r * Cols + k] * b.data_[k * Cols + c];
          }
          result.data_[r * Cols + c] = sum;
        }
      }
      return result;
    }

    // --- Scalar multiplication ---

    friend constexpr Matrix
    operator*(const Matrix& m, const T& s) {
      Matrix result;
      for (std::size_t i = 0; i < Rows * Cols; ++i) {
        result.data_[i] = m.data_[i] * s;
      }
      return result;
    }

    friend constexpr Matrix
    operator*(const T& s, const Matrix& m) {
      Matrix result;
      for (std::size_t i = 0; i < Rows * Cols; ++i) {
        result.data_[i] = s * m.data_[i];
      }
      return result;
    }

    // --- Scalar division ---

    friend constexpr Matrix
    operator/(const Matrix& m, const T& s) {
      Matrix result;
      for (std::size_t i = 0; i < Rows * Cols; ++i) {
        result.data_[i] = m.data_[i] / s;
      }
      return result;
    }

    // --- Compound assignment ---

    Matrix&
    operator+=(const Matrix& other) {
      return *this = *this + other;
    }

    Matrix&
    operator-=(const Matrix& other) {
      return *this = *this - other;
    }

    Matrix&
    operator*=(const Matrix& other)
      requires(Rows == Cols)
    {
      return *this = *this * other;
    }

    Matrix&
    operator*=(const T& s) {
      return *this = *this * s;
    }

    Matrix&
    operator/=(const T& s) {
      return *this = *this / s;
    }

    // --- Stream output ---

    friend std::ostream&
    operator<<(std::ostream& os, const Matrix& m) {
      os << "[";
      for (std::size_t r = 0; r < Rows; ++r) {
        if (r > 0) os << "; ";
        for (std::size_t c = 0; c < Cols; ++c) {
          if (c > 0) os << " ";
          os << m.data_[r * Cols + c];
        }
      }
      os << "]";
      return os;
    }

  private:
    std::array<T, Rows * Cols> data_;
  };

  // --- Free functions ---

  /// General matrix multiply: M×K times K×N → M×N.
  template <CommutativeRing T, std::size_t M, std::size_t K, std::size_t N>
  constexpr Matrix<T, M, N>
  matmul(const Matrix<T, M, K>& a, const Matrix<T, K, N>& b) {
    auto zero = identity_v<T, additive_tag>();
    Matrix<T, M, N> result;
    for (std::size_t r = 0; r < M; ++r) {
      for (std::size_t c = 0; c < N; ++c) {
        T sum = zero;
        for (std::size_t i = 0; i < K; ++i) {
          sum = sum + a(r, i) * b(i, c);
        }
        result(r, c) = sum;
      }
    }
    return result;
  }

  /// Transpose: M×N → N×M.
  template <CommutativeRing T, std::size_t M, std::size_t N>
  constexpr Matrix<T, N, M>
  transpose(const Matrix<T, M, N>& m) {
    Matrix<T, N, M> result;
    for (std::size_t r = 0; r < M; ++r) {
      for (std::size_t c = 0; c < N; ++c) {
        result(c, r) = m(r, c);
      }
    }
    return result;
  }

  /// Determinant via cofactor expansion along first row.
  template <CommutativeRing T, std::size_t N>
  constexpr T
  determinant(const Matrix<T, N, N>& m) {
    if constexpr (N == 1) {
      return m(0, 0);
    } else if constexpr (N == 2) {
      return m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0);
    } else {
      auto zero = identity_v<T, additive_tag>();
      auto one = identity_v<T, multiplicative_tag>();
      T result = zero;
      T sign = one;
      for (std::size_t c = 0; c < N; ++c) {
        auto sub =
          detail::submatrix<Matrix<T, N, N>, Matrix<T, N - 1, N - 1>>(m, 0, c);
        result = result + sign * m(0, c) * determinant(sub);
        sign = zero - sign;
      }
      return result;
    }
  }

  /// Trace: sum of diagonal elements.
  template <CommutativeRing T, std::size_t N>
  constexpr T
  trace(const Matrix<T, N, N>& m) {
    auto result = identity_v<T, additive_tag>();
    for (std::size_t i = 0; i < N; ++i) {
      result = result + m(i, i);
    }
    return result;
  }

  /// Cofactor matrix (matrix of signed minors).
  template <CommutativeRing T, std::size_t N>
  constexpr Matrix<T, N, N>
  cofactor_matrix(const Matrix<T, N, N>& m) {
    auto one = identity_v<T, multiplicative_tag>();
    auto zero = identity_v<T, additive_tag>();
    Matrix<T, N, N> result;
    for (std::size_t r = 0; r < N; ++r) {
      for (std::size_t c = 0; c < N; ++c) {
        auto sub =
          detail::submatrix<Matrix<T, N, N>, Matrix<T, N - 1, N - 1>>(m, r, c);
        T sign = ((r + c) % 2 == 0) ? one : (zero - one);
        result(r, c) = sign * determinant(sub);
      }
    }
    return result;
  }

  /// Inverse via adjugate / determinant.  Requires Field<T>.
  template <Field T, std::size_t N>
  constexpr Matrix<T, N, N>
  inverse(const Matrix<T, N, N>& m) {
    T det = determinant(m);
    auto adj = transpose(cofactor_matrix(m));
    return adj / det;
  }

  // --- identity_element specializations ---

  /// Additive identity: zero matrix (all dimensions).
  template <CommutativeRing T, std::size_t Rows, std::size_t Cols>
    requires(Rows > 0 && Cols > 0)
  struct identity_element<Matrix<T, Rows, Cols>, additive_tag> {
    static constexpr Matrix<T, Rows, Cols>
    make() {
      return Matrix<T, Rows, Cols>();
    }
  };

  /// Multiplicative identity: identity matrix (square only).
  template <CommutativeRing T, std::size_t N>
    requires(N > 0)
  struct identity_element<Matrix<T, N, N>, multiplicative_tag> {
    static constexpr Matrix<T, N, N>
    make() {
      return Matrix<T, N, N>(identity_v<T, multiplicative_tag>());
    }
  };

} // namespace arithkit
