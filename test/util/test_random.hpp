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

#pragma once

#include "../runner/check_xss.hpp"
#include "test_check.hpp"
#include "test_gen.hpp"

constexpr static uint8_t random_max_sigma = 128;
constexpr static uint64_t random_min_n = 64;
constexpr static uint64_t random_max_n_min = 4096;
constexpr static uint64_t random_max_n_max = 512 * 1024;

constexpr static uint64_t repetitions_per_config = 4;

using random_check_type = nss_check<false, true>;

template <uint64_t repetitions = repetitions_per_config>
static void random_test() {
  auto get_max_n = [&] (const uint64_t sigma) {
      return (uint64_t)std::min(random_max_n_max, random_max_n_min * sigma);
  };
  for (uint16_t sigma = 1; sigma <= random_max_sigma; sigma *= 2) {
    std::cout << "Sigma: " << uint64_t(sigma) << ", n = " << random_min_n;
    uint64_t mn = get_max_n(sigma);
    for (uint64_t n = random_min_n; n <= mn; n *= 2) {
      if(n > random_min_n) std::cout << ", " << n << std::flush;
      for (uint64_t r = 0; r < ((sigma == 1) ? 1 : repetitions); ++r) {
        auto instance = generate_test_random(n, sigma);
        check_all_xss_algos<random_check_type>(instance);
        std::reverse(instance.begin(), instance.end());
        check_all_xss_algos<random_check_type>(instance);
      }
    }
    std::cout << " [complete]" << std::endl;
  }
}