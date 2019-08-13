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

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#define xssr_always_inline __attribute__((always_inline)) inline
#define xssr_likely(x) __builtin_expect(!!(x), 1)
#define xssr_unlikely(x) __builtin_expect(!!(x), 0)

constexpr static uint64_t word_all_zero = 0ULL;
constexpr static uint64_t word_all_one = ~word_all_zero;
constexpr static uint64_t word_left_zero = word_all_one >> 1;
constexpr static uint64_t word_left_one = ~word_left_zero;
constexpr static uint64_t word_right_zero = word_all_one << 1;
constexpr static uint64_t word_right_one = ~word_right_zero;

template <uint64_t power_of_two>
constexpr xssr_always_inline static uint64_t div(const uint64_t value) {
  static_assert(__builtin_ctzl(power_of_two) < 64);
  static_assert((1ULL << __builtin_ctzl(power_of_two)) == power_of_two);
  return value >> __builtin_ctzl(power_of_two);
}

template <uint64_t power_of_two>
constexpr xssr_always_inline static uint64_t mul(const uint64_t value) {
  static_assert(__builtin_ctzl(power_of_two) < 64);
  static_assert((1ULL << __builtin_ctzl(power_of_two)) == power_of_two);
  return value << __builtin_ctzl(power_of_two);
}

template <uint64_t power_of_two>
constexpr xssr_always_inline static uint64_t mod(const uint64_t value) {
  static_assert(__builtin_ctzl(power_of_two) < 64);
  static_assert((1ULL << __builtin_ctzl(power_of_two)) == power_of_two);
  return value - mul<power_of_two>(div<power_of_two>(value));
}

constexpr xssr_always_inline static uint64_t div64(const uint64_t value) {
  return div<64>(value);
}

constexpr xssr_always_inline static uint64_t mul64(const uint64_t value) {
  return mul<64>(value);
}

constexpr xssr_always_inline static uint64_t div8(const uint64_t value) {
  return div<8>(value);
}

constexpr xssr_always_inline static uint64_t mul8(const uint64_t value) {
  return mul<8>(value);
}

constexpr xssr_always_inline static uint64_t mod64(const uint64_t value) {
  return mod<64>(value);
}