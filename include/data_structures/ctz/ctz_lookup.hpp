//  Copyright (c) 2019 Jonas Ellert
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to
//  deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
//  sell copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//  IN THE SOFTWARE.

#pragma once

#include <util/common.hpp>

template <uint64_t bitwidth>
struct ctz_linear;

template <uint64_t bitwidth>
struct ctz_logarithmic;

template <uint64_t bitwidth>
struct ctz_lookup {
  constexpr always_inline static uint64_t get(const uint64_t x) {
    // force compile time construction
    static_assert(instance.table_[1] == 0);
    return instance.table_[x];
  }

private:
  constexpr static ctz_lookup<bitwidth> instance = ctz_lookup<bitwidth>();
  constexpr static uint64_t b_ = bitwidth;

  constexpr ctz_lookup() : table_() {
    static_assert(b_ == 1 || b_ == 2 || b_ == 4 || b_ == 8 || b_ == 16);
    table_[0] = b_;
    for (uint64_t tz = 0; tz < b_; ++tz) {
      table_[1ULL << tz] = tz;
    }
  }
  uint8_t table_[1 + (1ULL << (b_ - 1))];
};

template <uint64_t bitwidth>
struct ctz_linear {
  constexpr always_inline static uint64_t get_safe(uint64_t x) {
    return (x > 0) ? ctz_internal(x & -x) : 64;
  }

  constexpr always_inline static uint64_t get_unsafe(uint64_t x) {
    return ctz_internal(x & -x);
  }

  static std::string to_string() {
    return "CTZ_LINEAR_" + std::to_string(bitwidth);
  }

private:
  using lookup = typename std::
      conditional<(bitwidth > 0), ctz_lookup<bitwidth>, bool>::type;

  constexpr ctz_linear() {}
  constexpr static uint64_t b_ = std::max(bitwidth, (uint64_t) 1);
  constexpr static uint64_t mask_ = (1ULL << b_) - 1;

  constexpr always_inline static uint64_t ctz_internal(uint64_t x) {
    uint64_t result = 0;
    while ((x & mask_) == 0) {
      x >>= b_;
      result += b_;
    }
    if constexpr (bitwidth == 0)
      return result;
    else
      return result + lookup::get(x & mask_);
  }
};

template <uint64_t bitwidth>
struct ctz_logarithmic {
  constexpr always_inline static uint64_t get_safe(uint64_t x) {
    return (x > 0) ? ctz_internal(x & -x) : 64;
  }

  constexpr always_inline static uint64_t get_unsafe(uint64_t x) {
    return ctz_internal(x & -x);
  }

  static std::string to_string() {
    return "CTZ_LOGARITHMIC_" + std::to_string(bitwidth);
  }

private:
  using lookup = typename std::
      conditional<(bitwidth > 0), ctz_lookup<bitwidth>, bool>::type;

  constexpr ctz_logarithmic() {}
  constexpr static uint64_t b_ = bitwidth >> 1;

  constexpr always_inline static uint64_t ctz_internal(uint64_t x) {
    uint64_t bits = 32;
    uint64_t mask = (1ULL << 32) - 1;
    uint64_t result = 0;
    while (bits > b_) {
      if ((x & mask) == 0) {
        result += bits;
        x >>= bits;
      }
      bits >>= 1;
      mask >>= bits;
    }
    if constexpr (bitwidth == 0)
      return result;
    else
      return result + lookup::get(x);
  }
};

struct ctz_debruijn {
  constexpr always_inline static uint64_t get_safe(const uint64_t x) {
    return (x > 0) ? lookup_[((x & -x) * martins_constant) >> 58] : 64;
  }

  constexpr always_inline static uint64_t get_unsafe(const uint64_t x) {
    return lookup_[((x & -x) * martins_constant) >> 58];
  }

  static std::string to_string() {
    return "CTZ_DEBRUIJN";
  }

private:
  constexpr static uint64_t martins_constant = 0x03f79d71b4ca8b09;
  constexpr static uint8_t lookup_[64] = {
      0,  1,  56, 2,  57, 49, 28, 3,  61, 58, 42, 50, 38, 29, 17, 4,
      62, 47, 59, 36, 45, 43, 51, 22, 53, 39, 33, 30, 24, 18, 12, 5,
      63, 55, 48, 27, 60, 41, 37, 16, 46, 35, 44, 21, 52, 32, 23, 11,
      54, 26, 40, 15, 34, 20, 31, 10, 25, 14, 19, 9,  13, 8,  7,  6};
  ctz_debruijn() {}
};
