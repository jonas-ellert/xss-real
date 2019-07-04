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

#include "runner/check_xss.hpp"
#include "util/test_check.hpp"
#include "util/test_gen.hpp"
#include "util/test_manual.hpp"
#include "util/test_lookahead.hpp"

constexpr static uint64_t min_n = 64;
constexpr static uint64_t max_n = 128ULL * 1024;
constexpr static uint64_t run_max_n = 1024;
constexpr static uint64_t run_of_runs_min_rep = 2;
constexpr static uint64_t run_of_runs_max_rep = 10;
constexpr static uint64_t run_of_runs_min_pow_rep = 16;
constexpr static uint64_t run_of_runs_max_pow_rep = 1024;

using check_type = nss_check<true, true>;

template <typename instance_collection>
static void hand_selected_test(instance_collection &&instances) {
  std::cout << "Number of instances: " << instances.size() << std::endl;
  uint64_t count = 0;
  for (auto instance : instances) {
    ++count;
    bool verbose = (instances.size() < 100 || (instances.size() < 1000 && (mod<16>(count)) == 0) || mod<128>(count) == 0);

    if (verbose)
      std::cout << "Testing instance " << count
                << " (of length " << instance.size() << "): " << std::flush;

    check_all_xss_algos<check_type>(instance);
    if (verbose) std::cout << "Forwards done. " << std::flush;

    std::reverse(instance.begin(), instance.end());
    check_all_xss_algos<check_type>(instance);
    if (verbose) std::cout << "Backwards done." << std::endl;
  }
}

static void run_a_test() {
  std::cout << "n = " << min_n;
  for (uint64_t n = min_n; n <= run_max_n; n *= 2) {
    if (n > min_n) std::cout << ", " << n << std::flush;
    auto instance = generate_test_run_a(n);
    check_all_xss_algos<check_type>(instance);

    instance[instance.size() - 2] = 'Z';
    check_all_xss_algos<check_type>(instance);
  }
  std::cout << " [complete]" << std::endl;
}

static void ababc_test(bool finish_with_z) {
  std::cout << "n = " << min_n;
  for(uint64_t n = min_n; n <= run_max_n; n *= 2) {
    if (n > min_n) std::cout << ", " << n << std::flush;
    auto instance = generate_test_ababc(n);
    if (finish_with_z) instance[instance.size() - 2] = 'z';
    check_all_xss_algos<check_type>(instance);

    std::reverse(instance.begin(), instance.end());
    check_all_xss_algos<check_type>(instance);
  }
  std::cout << " [complete]" << std::endl;
}

static void overlap_test() {
  std::cout << "n = " << min_n;
  for(uint64_t n = min_n; n <= max_n; n *= 2) {
    if (n > min_n) std::cout << ", " << n << std::flush;
    auto instance = generate_test_high_overlap(n);
    check_all_xss_algos<check_type>(instance);

    std::reverse(instance.begin(), instance.end());
    check_all_xss_algos<check_type>(instance);
  }
  std::cout << " [complete]" << std::endl;
}

static void run_of_runs_test() {
  for (uint8_t run_len =  run_of_runs_min_rep;
               run_len <= run_of_runs_max_rep;
               ++run_len) {

    std::cout << "Repetitions: " << uint64_t(run_len) << ", n = " << min_n;
    for(uint64_t n = min_n; n <= max_n; n *= 2) {
      if (n > min_n) std::cout << ", " << std::flush;
      auto instance = generate_test_run_of_runs(n, run_len);
      if (n > min_n) std::cout << n << std::flush;
      check_all_xss_algos<check_type>(instance);

      std::reverse(instance.begin(), instance.end());
      check_all_xss_algos<check_type>(instance);
    }
    std::cout << " [complete]" << std::endl;
  }

  for (uint64_t run_len = run_of_runs_min_pow_rep;
       run_len <= run_of_runs_max_pow_rep;
       run_len *= 2) {

    std::cout << "Repetitions: " << uint64_t(run_len) << ", n = " << min_n;
    for(uint64_t n = min_n; n <= max_n; n *= 2) {
      if (n > min_n) std::cout << ", " << std::flush;
      auto instance = generate_test_run_of_runs(n, run_len);
      if (n > min_n) std::cout << n << std::flush;
      check_all_xss_algos<check_type>(instance);

      std::reverse(instance.begin(), instance.end());
      check_all_xss_algos<check_type>(instance);
    }
    std::cout << " [complete]" << std::endl;
  }
}

TEST(xss, hand_selected) {
  std::cout << "Testing XSS with hand selected instances." << std::endl;
  hand_selected_test(manual_test_instances());
}

TEST(xss, lookahead) {
  std::cout << "Testing XSS with hand selected instances "
            <<"(cover all lookahead cases)." << std::endl;
  hand_selected_test(manual_test_instances_lookahead(512));
}

TEST(xss, run_a) {
  std::cout << "Testing XSS with run of 'a'." << std::endl;
  run_a_test();
}


TEST(xss, high_overlap) {
  std::cout << "Testing XSS with strongly overlapping strings." << std::endl;
  std::cout << "[Pattern: " << generate_test_high_overlap(30) << "]" << std::endl;
  overlap_test();
}


TEST(xss, run_of_runs) {
  std::cout << "Testing XSS with run of runs." << std::endl;
  std::cout << "[Pattern (example for period 3): "
            << generate_test_run_of_runs(42, 3) << "]" << std::endl;
  run_of_runs_test();
}

TEST(xss, ababc) {
  std::cout << "Testing XSS with \"ababc\" run." << std::endl;
  ababc_test(false);
}

TEST(xss, ababc2) {
  std::cout << "Testing XSS with \"ababc\" run (terminate run with z)." << std::endl;
  ababc_test(true);
}
