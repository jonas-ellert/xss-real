//  Copyright (c) 2019 Jonas Ellert
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.

#include <gtest/gtest.h>

#include <random>
#include <bitset>
#include <util/ctz/ctz.hpp>

struct rng_type {
  constexpr static uint64_t min = 0;
  constexpr static uint64_t max = std::numeric_limits<uint64_t>::max();
  std::random_device rd;
  std::mt19937 eng;
  std::uniform_int_distribution<uint64_t> distr;

  rng_type() : eng(rd()), distr(min, max) {}

  uint64_t operator()() {
    return distr(eng);
  }
} rng;

template <typename ctz_type, uint64_t tz>
static void sanity_check() {
  if constexpr (tz == 64)
    static_assert(ctz_type::get(0) == 64);
  else {
    static_assert(ctz_type::get(1ULL << tz) == tz);
  }
  if constexpr (tz > 0) {
    sanity_check<ctz_type, tz - 1>();
  }
}

template <typename ctz_type>
static void random_test_ctz() {

  std::cout << "Testing lookup for strategy " << ctz_type::to_string()
            << "..." << std::endl;

//  sanity_check<ctz_type, 64>();

  constexpr uint64_t max = std::numeric_limits<uint64_t>::max();
  for (uint64_t i = 0; i < 100000; ++i) {
    uint64_t value = rng();
    for (int j = 0; j < 64; ++j) {
      value = value & (max << j);
      EXPECT_EQ(ctz_builtin::get(value), ctz_type::get(value));
    }
  }
}


TEST(ctz_lookup, random_test) {
  //random_test_ctz<CTZ_BUILTIN>();
  random_test_ctz<typename ctz_auto<CTZ_DEBRUIJN>::type>();
  random_test_ctz<typename ctz_auto<CTZ_LINEAR, 0>::type>();
  random_test_ctz<typename ctz_auto<CTZ_LINEAR, 1>::type>();
  random_test_ctz<typename ctz_auto<CTZ_LINEAR, 2>::type>();
  random_test_ctz<typename ctz_auto<CTZ_LINEAR, 4>::type>();
  random_test_ctz<typename ctz_auto<CTZ_LINEAR, 8>::type>();
  random_test_ctz<typename ctz_auto<CTZ_LINEAR, 16>::type>();
  random_test_ctz<typename ctz_auto<CTZ_LOGARITHMIC, 0>::type>();
  random_test_ctz<typename ctz_auto<CTZ_LOGARITHMIC, 1>::type>();
  random_test_ctz<typename ctz_auto<CTZ_LOGARITHMIC, 2>::type>();
  random_test_ctz<typename ctz_auto<CTZ_LOGARITHMIC, 4>::type>();
  random_test_ctz<typename ctz_auto<CTZ_LOGARITHMIC, 8>::type>();
  random_test_ctz<typename ctz_auto<CTZ_LOGARITHMIC, 16>::type>();
}

