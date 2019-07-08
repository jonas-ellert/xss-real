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
#include <iterator>
#include <fstream>
#include <streambuf>

#include <omp.h>

#ifdef MALLOC_COUNT
#include <malloc_count.h>
#else
#include <MALLOC COUNT NOT ENABLED IN CMAKE>
#endif // MALLOC_COUNT

#include <algorithm/xss_real.hpp>
#include <util/time_measure.hpp>
#include "../util/test_gen.hpp"
#include "../util/test_files.hpp"

constexpr static uint64_t n = 64ULL * 1024 * 1024;
constexpr static uint64_t run_of_runs_min_period = 2;
constexpr static uint64_t run_of_runs_max_period = 10;


template <stack_strategy strategy, ds_direction_flag direction, typename input_type>
static void test_mem_time_internal(const input_type &input, const uint64_t delta) {
  time_measure time_measurement;
  malloc_count_reset_peak();
  uint64_t mem_pre = malloc_count_current();
  time_measurement.begin();

  auto res = xss_real<strategy>::run(input.data(), input.size(), delta);

  time_measurement.end();
  uint64_t mem_peak = malloc_count_peak();

  const auto input_size = (input.size() / 1024.0 / 1024);
  const auto peak = (mem_peak - mem_pre) / 1024.0 / 1024;
  const auto bpn = (8.0 * (mem_peak - mem_pre)) / (input.size());
  const auto time = time_measurement.millis() / 1000.0;
  const auto speed = input_size / time;

  std::cout << std::to_string(strategy) << "_" << delta << ": "
            << "n = " << input_size << "MB"
            << ", mem = " << peak << "MB (" << bpn << " bpn)"
            << ", time = " << time << "s"
            << ", speed = " << speed << "MB/s"
            << std::endl;
}

template <stack_strategy strategy, ds_direction_flag direction, typename input_type>
static void test_mem_time_internal(const input_type &input) {
  test_mem_time_internal<strategy, direction>(input, 0);
  if constexpr (strategy != NAIVE) {
    for (uint64_t delta = 1; delta <= 32; delta = delta << 1) {
      test_mem_time_internal<strategy, direction>(input, delta);
    }
  }
}


template <ds_direction_flag direction, typename input_type>
static void test_mem_time(const input_type &input) {
  test_mem_time_internal<NAIVE, direction>(input);
  test_mem_time_internal<STATIC, direction>(input);
  test_mem_time_internal<DYNAMIC, direction>(input);
  test_mem_time_internal<DYNAMIC_BUFFERED, direction>(input);
}


template <ds_direction_flag direction>
static void overlap_test() {
  std::vector<uint8_t> instance = instance_from_file("high_overlap", n);
  if (instance.size() == 0) {
    instance = generate_test_high_overlap(n);
    instance_to_file("high_overlap", instance, n);
  }
  test_mem_time<direction>(instance);
  std::reverse(instance.begin(), instance.end());
  test_mem_time<direction>(instance);
}

template <ds_direction_flag direction>
static void run_of_runs_test() {
  for (uint8_t run_len =  run_of_runs_min_period;
               run_len <= run_of_runs_max_period;
               ++run_len) {

    std::cout << "Period: " << uint64_t(run_len) << std::endl;

    std::string instance_name = "run_of_runs_" + std::to_string(uint64_t(run_len));
    std::vector<uint8_t> instance = instance_from_file(instance_name, n);
    if (instance.size() == 0) {
      instance = generate_test_run_of_runs(n, run_len);
      instance_to_file(instance_name, instance, n);
    }
    test_mem_time<direction>(instance);
//    std::reverse(instance.begin(), instance.end());
    instance[instance.size() - 1] = 'Z';
    test_mem_time<direction>(instance);
  }
}

template <ds_direction_flag direction>
static void ababc_test(bool finish_with_z) {
  std::vector<uint8_t> instance = instance_from_file("ababc", n);
  if (instance.size() == 0) {
    instance = generate_test_ababc(n);
    instance_to_file("ababc", instance, n);
  }
  if (finish_with_z)
    instance[instance.size() - 2] = 'z';
  test_mem_time<direction>(instance);
  std::reverse(instance.begin(), instance.end());
  test_mem_time<direction>(instance);
}

template <ds_direction_flag direction>
static void run_a_test() {
  std::vector<uint8_t> instance = instance_from_file("run_a", n);
  if (instance.size() == 0) {
    instance.reserve(n);
    instance.push_back('.');
    for (uint64_t i = 0; i < n - 2; ++i) {
      instance.push_back('a');
    }
    instance.push_back('.');
    instance_to_file("run_a", instance, n);
  }
  test_mem_time<direction>(instance);
  instance[instance.size() - 2] = 'z';
  test_mem_time<direction>(instance);
}

template <ds_direction_flag direction>
static void random_test() {
  for (uint8_t sigma = 2; sigma <= 8 ; sigma *= 2) {
    std::string instance_name = "random_sigma" + std::to_string(uint64_t(sigma));
    std::vector<uint8_t> instance = instance_from_file(instance_name, n);
    if (instance.size() == 0) {
      instance = generate_test_random(n, sigma);
      instance_to_file(instance_name, instance, n);
    }
    test_mem_time<direction>(instance);
  }
}

//TEST(nearest_smaller_suffix_ltr, next_random) {
//  std::cout << "Testing NSS with random input." << std::endl;
//  random_test<NEXT>();
//}
//
//TEST(nearest_smaller_suffix_ltr, previous_random) {
//  std::cout << "Testing PSS with random input." << std::endl;
//  random_test<PREVIOUS>();
//}

TEST(nearest_smaller_suffix_ltr, previous_run_a) {
  std::cout << "Testing PSS with run of 'a'." << std::endl;
  run_a_test<PREVIOUS>();
}

TEST(nearest_smaller_suffix_ltr, previous_high_overlap) {
  std::cout << "Testing PSS with strongly overlapping strings." << std::endl;
  std::cout << "[Pattern: " << generate_test_high_overlap(32) << "]" << std::endl;
  overlap_test<PREVIOUS>();
}

TEST(nearest_smaller_suffix_ltr, previous_run_of_runs) {
  std::cout << "Testing PSS with run of runs." << std::endl;
  std::cout << "[Pattern: " << generate_test_run_of_runs(30, 3) << "]" << std::endl;
  run_of_runs_test<PREVIOUS>();
}

TEST(nearest_smaller_suffix_ltr, previous_ababc) {
  std::cout << "Testing PSS with \"ababc\" run." << std::endl;
  ababc_test<PREVIOUS>(false);
}

TEST(nearest_smaller_suffix_ltr, previous_ababc2) {
  std::cout << "Testing PSS with \"ababc\" run (finish with z)." << std::endl;
  ababc_test<PREVIOUS>(true);
}

TEST(force_abort, force_abort) {
  ASSERT_EQ("FORCE", "ABORT");
}