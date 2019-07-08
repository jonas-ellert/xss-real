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

#include "../runner/check_xss.hpp"
#include "../util/test_check.hpp"

constexpr static uint8_t max_sigma = 128;
constexpr static uint64_t max_n = 10 * 1024;
constexpr static uint64_t max_n_s1 = 4 * 1024;
constexpr static uint64_t run_of_runs_period = 4;

using check_type = nss_check<false, true>;

template <ds_direction_flag direction>
static void test_all() {
  uint64_t n = 20;
  constexpr static uint8_t min_char = 65;
  for (uint16_t sigma = 2; sigma <= 5; ++sigma) {
    n -= 2;
    std::cout << "Building balanced parentheses (sigma: " << uint64_t(sigma) << ", n = " << n << ")..." << std::endl;
    const uint8_t max_char = min_char + sigma - 1;
    std::vector<uint8_t> array(n);
    for (uint64_t j = 0; j < n; ++j) array[j] = min_char;
    array[0] = '\0';
    array[n - 1] = '\0';
    while (true) {
      check_all_xss_algos<direction, check_type>(array);

      uint64_t idx_for_incr = n - 2;
      while (array[idx_for_incr] == max_char) {
        array[idx_for_incr] = min_char;
        --idx_for_incr;
      }

      if (idx_for_incr == 0) break;
      ++array[idx_for_incr];
    }
  }
}

TEST(nearest_smaller_suffix_ltr, next_all_possible) {
  test_all<NEXT>();
}

TEST(nearest_smaller_suffix_ltr, previous_all_possible) {
  test_all<PREVIOUS>();
}

