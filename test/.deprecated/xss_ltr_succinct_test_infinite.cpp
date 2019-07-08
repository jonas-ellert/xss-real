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

#include <cmath>
#include <random>

#include "../runner/check_xss.hpp"
#include "../util/test_check.hpp"
#include "../util/test_gen.hpp"

constexpr static uint8_t max_sigma = 4;
constexpr static uint64_t min_n = 4;
constexpr static uint64_t max_n = 32ULL * 1024;//32 * 1024;

using check_type = nss_check<false, true>;

inline static std::string sigma_to_string(const uint8_t sigma) {
  const static uint64_t pad_sigma = std::to_string(uint64_t(max_sigma)).size();
  std::string result = std::to_string(uint64_t(sigma));
  result.insert(result.begin(), pad_sigma - result.length(), ' ');
  return result;
}

inline static std::string n_to_string(const uint64_t n) {
  const static uint64_t pad_n = std::to_string(max_n).size();
  std::string result = std::to_string(n);
  result.insert(result.begin(), pad_n - result.length(), ' ');
  return result;
}

template <ds_direction_flag direction, bool both_directions>
static void infinite_random_test() {
  constexpr ds_direction_flag direction_or_NEXT = both_directions ? NEXT : direction;

  std::random_device rd;     // only used once to initialise (seed) engine
  std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
  std::uniform_int_distribution<unsigned int> uni_sigma(2, max_sigma); // guaranteed unbiased
  auto rnd_sigma = [&](){ return uint8_t(uni_sigma(rng)); };
  std::uniform_int_distribution<unsigned int> uni_n(min_n, max_n); // guaranteed unbiased
  auto rnd_n = [&](){ return uint64_t(uni_n(rng)); };

  std::cout << "Starting infinite random test with sigma from [2, " << uint64_t(max_sigma)
            << "] and n from [1, 2^" << std::log2(max_n) << "]..." << std::endl;

  uint64_t instance_count = 0;
  while(true) {
    uint8_t sigma = rnd_sigma();;
    uint64_t n = rnd_n();

    std::cout << "\rTested on " << instance_count << " random texts. "
              << "Next sigma: " << sigma_to_string(sigma) << ", next n: " << n_to_string(n);

    auto instance = generate_test_random(n, sigma);
    check_all_xss_algos<direction_or_NEXT, check_type>(instance);
    if constexpr (both_directions)
      check_all_xss_algos<PREVIOUS, check_type>(instance);

    std::reverse(instance.begin(), instance.end());
    check_all_xss_algos<direction_or_NEXT, check_type>(instance);
    if constexpr (both_directions)
      check_all_xss_algos<PREVIOUS, check_type>(instance);

    ++instance_count;
  }
}

TEST(nearest_smaller_suffix_ltr, next_and_previous_random_infinite) {
  infinite_random_test<NEXT, true>();
}

TEST(nearest_smaller_suffix_ltr, next_random_infinite) {
  infinite_random_test<NEXT, false>();
}

TEST(nearest_smaller_suffix_ltr, previous_random_infinite) {
  infinite_random_test<PREVIOUS, false>();
}