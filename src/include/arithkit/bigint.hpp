#pragma once

/// @file bigint.hpp
/// @brief Arbitrary-precision integer (BigInt).
///
/// Models EuclideanDomain. Internal representation: sign + magnitude
/// stored as a vector of base-2^32 limbs (least significant first).

#include <algorithm>
#include <compare>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

#include <arithkit/traits/identity_element.hpp>

namespace arithkit {

  class BigInt {
  public:
    using limb_t = uint32_t;
    using dlimb_t = uint64_t;
    static constexpr dlimb_t base = dlimb_t(1) << 32;

    // --- Construction ---

    BigInt()
        : negative_(false)
        , limbs_{0} {}

    BigInt(long long val)
        : negative_(val < 0) {
      auto abs_val =
        static_cast<uint64_t>(negative_ ? -static_cast<int64_t>(val) : val);
      if (abs_val == 0) {
        limbs_ = {0};
      } else {
        limbs_.clear();
        while (abs_val > 0) {
          limbs_.push_back(static_cast<limb_t>(abs_val & 0xFFFFFFFF));
          abs_val >>= 32;
        }
      }
    }

    // --- Comparison ---

    friend bool
    operator==(const BigInt& a, const BigInt& b) {
      if (a.is_zero() && b.is_zero()) return true;
      return a.negative_ == b.negative_ && a.limbs_ == b.limbs_;
    }

    friend std::strong_ordering
    operator<=>(const BigInt& a, const BigInt& b) {
      if (a.is_zero() && b.is_zero()) return std::strong_ordering::equal;
      if (a.negative_ != b.negative_)
        return a.negative_ ? std::strong_ordering::less
                           : std::strong_ordering::greater;
      auto mag = compare_magnitude(a, b);
      if (a.negative_) return 0 <=> mag; // reversed for negatives
      return mag <=> 0;
    }

    // --- Unary ---

    BigInt
    operator-() const {
      if (is_zero()) return *this;
      BigInt result = *this;
      result.negative_ = !result.negative_;
      return result;
    }

    BigInt
    operator+() const {
      return *this;
    }

    // --- Addition ---

    friend BigInt
    operator+(const BigInt& a, const BigInt& b) {
      if (a.negative_ == b.negative_)
        return from_parts(a.negative_, add_magnitudes(a, b));

      int cmp = compare_magnitude(a, b);
      if (cmp == 0) return BigInt(0);
      if (cmp > 0) return from_parts(a.negative_, sub_magnitudes(a, b));
      return from_parts(b.negative_, sub_magnitudes(b, a));
    }

    // --- Subtraction ---

    friend BigInt
    operator-(const BigInt& a, const BigInt& b) {
      return a + (-b);
    }

    // --- Multiplication ---

    friend BigInt
    operator*(const BigInt& a, const BigInt& b) {
      if (a.is_zero() || b.is_zero()) return BigInt(0);
      auto result_limbs = multiply_magnitudes(a, b);
      return from_parts(a.negative_ != b.negative_, std::move(result_limbs));
    }

    // --- Division and modulus ---

    friend BigInt
    operator/(const BigInt& a, const BigInt& b) {
      auto [q, r] = divmod(a, b);
      return q;
    }

    friend BigInt
    operator%(const BigInt& a, const BigInt& b) {
      auto [q, r] = divmod(a, b);
      return r;
    }

    // --- Compound assignment ---

    BigInt&
    operator+=(const BigInt& other) {
      return *this = *this + other;
    }
    BigInt&
    operator-=(const BigInt& other) {
      return *this = *this - other;
    }
    BigInt&
    operator*=(const BigInt& other) {
      return *this = *this * other;
    }
    BigInt&
    operator/=(const BigInt& other) {
      return *this = *this / other;
    }
    BigInt&
    operator%=(const BigInt& other) {
      return *this = *this % other;
    }

    // --- Queries ---

    [[nodiscard]] bool
    is_zero() const {
      return limbs_.size() == 1 && limbs_[0] == 0;
    }

    [[nodiscard]] bool
    is_negative() const {
      return negative_ && !is_zero();
    }

    // --- String conversion ---

    friend std::string
    to_string(const BigInt& n) {
      if (n.is_zero()) return "0";

      BigInt abs_n = n.is_negative() ? -n : n;
      std::string digits;
      BigInt ten(10);

      while (!abs_n.is_zero()) {
        auto [q, r] = divmod_unsigned(abs_n, ten);
        digits.push_back(static_cast<char>('0' + r.limbs_[0]));
        abs_n = std::move(q);
      }

      if (n.is_negative()) digits.push_back('-');
      std::reverse(digits.begin(), digits.end());
      return digits;
    }

    friend std::ostream&
    operator<<(std::ostream& os, const BigInt& n) {
      return os << to_string(n);
    }

    // --- GCD ---

    friend BigInt
    gcd(BigInt a, BigInt b) {
      a = abs(a);
      b = abs(b);
      while (!b.is_zero()) {
        auto [q, r] = divmod_unsigned(a, b);
        a = std::move(b);
        b = std::move(r);
      }
      return a;
    }

    friend BigInt
    abs(const BigInt& n) {
      if (n.is_negative()) return -n;
      return n;
    }

  private:
    bool negative_;
    std::vector<limb_t> limbs_; // least significant first

    // --- Internal helpers ---

    static BigInt
    from_parts(bool neg, std::vector<limb_t> limbs) {
      BigInt result;
      result.negative_ = neg;
      result.limbs_ = std::move(limbs);
      result.normalize();
      return result;
    }

    void
    normalize() {
      while (limbs_.size() > 1 && limbs_.back() == 0)
        limbs_.pop_back();
      if (is_zero()) negative_ = false;
    }

    static int
    compare_magnitude(const BigInt& a, const BigInt& b) {
      if (a.limbs_.size() != b.limbs_.size())
        return a.limbs_.size() < b.limbs_.size() ? -1 : 1;
      for (auto i = a.limbs_.size(); i-- > 0;) {
        if (a.limbs_[i] != b.limbs_[i])
          return a.limbs_[i] < b.limbs_[i] ? -1 : 1;
      }
      return 0;
    }

    static std::vector<limb_t>
    add_magnitudes(const BigInt& a, const BigInt& b) {
      const auto& longer =
        a.limbs_.size() >= b.limbs_.size() ? a.limbs_ : b.limbs_;
      const auto& shorter =
        a.limbs_.size() >= b.limbs_.size() ? b.limbs_ : a.limbs_;
      std::vector<limb_t> result(longer.size() + 1);
      dlimb_t carry = 0;

      for (size_t i = 0; i < longer.size(); ++i) {
        dlimb_t sum = carry + longer[i];
        if (i < shorter.size()) sum += shorter[i];
        result[i] = static_cast<limb_t>(sum);
        carry = sum >> 32;
      }
      if (carry) result[longer.size()] = static_cast<limb_t>(carry);

      // trim
      while (result.size() > 1 && result.back() == 0)
        result.pop_back();
      return result;
    }

    // Precondition: |a| >= |b|
    static std::vector<limb_t>
    sub_magnitudes(const BigInt& a, const BigInt& b) {
      std::vector<limb_t> result(a.limbs_.size());
      dlimb_t borrow = 0;

      for (size_t i = 0; i < a.limbs_.size(); ++i) {
        dlimb_t sub = static_cast<dlimb_t>(a.limbs_[i]) - borrow;
        if (i < b.limbs_.size()) sub -= b.limbs_[i];
        if (sub > base - 1) {
          // underflow: borrow
          sub += base;
          borrow = 1;
        } else {
          borrow = 0;
        }
        result[i] = static_cast<limb_t>(sub);
      }

      while (result.size() > 1 && result.back() == 0)
        result.pop_back();
      return result;
    }

    static std::vector<limb_t>
    multiply_magnitudes(const BigInt& a, const BigInt& b) {
      std::vector<limb_t> result(a.limbs_.size() + b.limbs_.size(), 0);

      for (size_t i = 0; i < a.limbs_.size(); ++i) {
        dlimb_t carry = 0;
        for (size_t j = 0; j < b.limbs_.size(); ++j) {
          dlimb_t product = static_cast<dlimb_t>(a.limbs_[i]) *
                              static_cast<dlimb_t>(b.limbs_[j]) +
                            result[i + j] + carry;
          result[i + j] = static_cast<limb_t>(product);
          carry = product >> 32;
        }
        result[i + b.limbs_.size()] += static_cast<limb_t>(carry);
      }

      while (result.size() > 1 && result.back() == 0)
        result.pop_back();
      return result;
    }

    // Division: returns {quotient, remainder} for non-negative
    // inputs. Uses schoolbook long division.
    static std::pair<BigInt, BigInt>
    divmod_unsigned(const BigInt& a, const BigInt& b) {
      if (b.is_zero()) {
        // Undefined — return zero for safety
        return {BigInt(0), BigInt(0)};
      }

      int cmp = compare_magnitude(a, b);
      if (cmp < 0) return {BigInt(0), a};
      if (cmp == 0) return {BigInt(1), BigInt(0)};

      if (b.limbs_.size() == 1) return divmod_single(a, b.limbs_[0]);

      return divmod_knuth(a, b);
    }

    // Fast path: single-limb divisor
    static std::pair<BigInt, BigInt>
    divmod_single(const BigInt& a, limb_t d) {
      std::vector<limb_t> q(a.limbs_.size());
      dlimb_t rem = 0;

      for (auto i = a.limbs_.size(); i-- > 0;) {
        dlimb_t cur = (rem << 32) | a.limbs_[i];
        q[i] = static_cast<limb_t>(cur / d);
        rem = cur % d;
      }

      while (q.size() > 1 && q.back() == 0)
        q.pop_back();

      BigInt quotient;
      quotient.limbs_ = std::move(q);
      BigInt remainder(static_cast<long long>(rem));
      return {quotient, remainder};
    }

    // Knuth's Algorithm D for multi-limb division
    static std::pair<BigInt, BigInt>
    divmod_knuth(const BigInt& u, const BigInt& v) {
      auto n = v.limbs_.size();
      auto m = u.limbs_.size() - n;

      // Normalize: shift so top bit of v's MSL is set
      int shift = 0;
      auto vn_top = v.limbs_[n - 1];
      while ((vn_top & 0x80000000u) == 0) {
        vn_top <<= 1;
        shift++;
      }

      // Create normalized copies
      std::vector<limb_t> un(u.limbs_.size() + 1, 0);
      std::vector<limb_t> vn(n, 0);

      if (shift > 0) {
        for (size_t i = n - 1; i > 0; --i)
          vn[i] = static_cast<limb_t>(
            (static_cast<dlimb_t>(v.limbs_[i]) << shift) |
            (static_cast<dlimb_t>(v.limbs_[i - 1]) >> (32 - shift)));
        vn[0] = static_cast<limb_t>(static_cast<dlimb_t>(v.limbs_[0]) << shift);

        un[u.limbs_.size()] = static_cast<limb_t>(
          static_cast<dlimb_t>(u.limbs_[u.limbs_.size() - 1]) >> (32 - shift));
        for (size_t i = u.limbs_.size() - 1; i > 0; --i)
          un[i] = static_cast<limb_t>(
            (static_cast<dlimb_t>(u.limbs_[i]) << shift) |
            (static_cast<dlimb_t>(u.limbs_[i - 1]) >> (32 - shift)));
        un[0] = static_cast<limb_t>(static_cast<dlimb_t>(u.limbs_[0]) << shift);
      } else {
        for (size_t i = 0; i < u.limbs_.size(); ++i)
          un[i] = u.limbs_[i];
        for (size_t i = 0; i < n; ++i)
          vn[i] = v.limbs_[i];
      }

      std::vector<limb_t> q(m + 1, 0);

      for (auto j = m + 1; j-- > 0;) {
        // Estimate quotient digit
        dlimb_t qhat =
          ((static_cast<dlimb_t>(un[j + n]) << 32) | un[j + n - 1]) / vn[n - 1];
        dlimb_t rhat =
          ((static_cast<dlimb_t>(un[j + n]) << 32) | un[j + n - 1]) % vn[n - 1];

        while (qhat >= base ||
               (n >= 2 && qhat * vn[n - 2] > (rhat << 32) + un[j + n - 2])) {
          qhat--;
          rhat += vn[n - 1];
          if (rhat >= base) break;
        }

        // Multiply and subtract
        int64_t borrow_s = 0;
        for (size_t i = 0; i < n; ++i) {
          dlimb_t product = qhat * vn[i];
          int64_t diff = static_cast<int64_t>(un[j + i]) - borrow_s -
                         static_cast<int64_t>(static_cast<limb_t>(product));
          un[j + i] = static_cast<limb_t>(diff);
          borrow_s = static_cast<int64_t>(product >> 32) - (diff >> 32);
        }
        int64_t diff_top = static_cast<int64_t>(un[j + n]) - borrow_s;
        un[j + n] = static_cast<limb_t>(diff_top);

        q[j] = static_cast<limb_t>(qhat);

        // If negative, add back
        if (diff_top < 0) {
          q[j]--;
          dlimb_t carry = 0;
          for (size_t i = 0; i < n; ++i) {
            dlimb_t sum = static_cast<dlimb_t>(un[j + i]) + vn[i] + carry;
            un[j + i] = static_cast<limb_t>(sum);
            carry = sum >> 32;
          }
          un[j + n] += static_cast<limb_t>(carry);
        }
      }

      // Unnormalize remainder
      std::vector<limb_t> r(n, 0);
      if (shift > 0) {
        for (size_t i = 0; i < n - 1; ++i)
          r[i] = static_cast<limb_t>(
            (static_cast<dlimb_t>(un[i]) >> shift) |
            (static_cast<dlimb_t>(un[i + 1]) << (32 - shift)));
        r[n - 1] =
          static_cast<limb_t>(static_cast<dlimb_t>(un[n - 1]) >> shift);
      } else {
        for (size_t i = 0; i < n; ++i)
          r[i] = un[i];
      }

      while (q.size() > 1 && q.back() == 0)
        q.pop_back();
      while (r.size() > 1 && r.back() == 0)
        r.pop_back();

      BigInt quotient;
      quotient.limbs_ = std::move(q);
      BigInt remainder;
      remainder.limbs_ = std::move(r);
      return {quotient, remainder};
    }

    // Signed divmod: truncated division (C++ semantics)
    // a == (a/b)*b + a%b, sign of remainder matches dividend
    static std::pair<BigInt, BigInt>
    divmod(const BigInt& a, const BigInt& b) {
      auto [q, r] = divmod_unsigned(abs(a), abs(b));
      if (a.is_negative() != b.is_negative() && !q.is_zero()) q = -q;
      if (a.is_negative() && !r.is_zero()) r = -r;
      return {q, r};
    }
  };

  // --- identity_element specializations ---

  template <>
  struct identity_element<BigInt, additive_tag> {
    static BigInt
    make() {
      return BigInt(0);
    }
  };

  template <>
  struct identity_element<BigInt, multiplicative_tag> {
    static BigInt
    make() {
      return BigInt(1);
    }
  };

} // namespace arithkit
