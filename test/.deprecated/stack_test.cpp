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
#include <util/random.hpp>
#include <data_structures/lce/lce_naive.hpp>
#include <data_structures/stacks/lcp_stack/lcp_stack.hpp>


template <typename stack_type>
static void bench_single_stack(const uint64_t n,
                               const uint8_t* text,
                               const uint64_t delta,
                               const std::vector<uint64_t>& indices,
                               const std::vector<uint64_t>& lcps) {

  std::cout << "Testing " << typeid(stack_type).name() << std::endl;

  stack_type stack(n, delta, text);

  for (uint64_t i = 0; i < indices.size(); ++i) {
//    std::cout << "Push " << indices[i] << " " << lcps[i] << std::endl;
    stack.push(indices[i], lcps[i]);
  }

  for (uint64_t i = indices.size(); i > 0;) {
    --i;
    EXPECT_EQ(stack.top_idx(), indices[i]) << "Not equal for index " << i;
    EXPECT_EQ(stack.top_lcp(), lcps[i]) << "Not equal for index " << i;
    stack.pop();
  }
}

static void test_stacks(const uint64_t n, const uint64_t range, const uint64_t runs) {

  random_number_generator<uint64_t> rngmax;
  random_number_generator<uint64_t> rng2(0, 1);

  for (uint64_t r = 0; r < runs; ++r) {

    std::vector<uint64_t> lcp_values;
    lcp_values.reserve(n);
    for (uint64_t i = 0; i < (n >> 1); ++i) {
      const uint64_t first = rngmax() % range;
      lcp_values.push_back(first);
      lcp_values.push_back(
          (rng2()) ? (first + (rngmax() % range))
                   : (first - (rngmax() % (std::min(range, first + 1)))));
    }

    std::vector<uint64_t> indices;
    indices.reserve(n);

    std::vector<uint8_t> str;
    for (uint64_t i = 0; i < range; ++i) {
      str.push_back(rng2());
    }

    uint64_t str_i;
    uint64_t prev_str_i = 0;
    for (uint64_t i = 0; i < lcp_values.size(); ++i) {
      str_i = str.size();
      for (uint64_t j = 0; j < lcp_values[i]; ++j) {
        str.push_back(str[prev_str_i + j]); // force lcp
      }
      str.push_back(1 - str[prev_str_i + lcp_values[i]]); // force mistmatch

      indices.push_back(str_i);
      prev_str_i = str_i;
    }
    str.push_back('\0');

    // verify data
    auto lce = lce_naive<uint8_t>::get_lce(str.data());
    for (uint64_t k = 1; k < lcp_values.size(); ++k) {
      ASSERT_EQ(lce(indices[k - 1], indices[k]), lcp_values[k]);
      ASSERT_LT(indices[k - 1], indices[k]);
    }

    std::cout << "Prepared data. " << indices[0] << std::endl;

    std::cout << "n = " << indices.size() << ", delta = " << 0 << std::endl;
    // plain types
    bench_single_stack<typename lcp_stack<NAIVE, ctz_builtin, false, uint8_t>::type>(str.size(), str.data(), 0, indices, lcp_values);
    bench_single_stack<typename lcp_stack<STATIC, ctz_builtin, false, uint8_t>::type>(str.size(), str.data(), 0, indices, lcp_values);
    bench_single_stack<typename lcp_stack<DYNAMIC, ctz_builtin, false, uint8_t>::type>(str.size(), str.data(), 0, indices, lcp_values);
    bench_single_stack<typename lcp_stack<DYNAMIC_BUFFERED, ctz_builtin, false, uint8_t>::type>(str.size(), str.data(), 0, indices, lcp_values);

    //delta types
    for (uint64_t delta = 1; delta <= 32; delta <<= 1) {
      std::cout << "n = " << indices.size() << ", delta = " << delta << std::endl;
      bench_single_stack<typename lcp_stack<STATIC, ctz_builtin, true, uint8_t>::type>(str.size(), str.data(), delta, indices, lcp_values);
      bench_single_stack<typename lcp_stack<DYNAMIC, ctz_builtin, true, uint8_t>::type>(str.size(), str.data(), delta, indices, lcp_values);
      bench_single_stack<typename lcp_stack<DYNAMIC_BUFFERED, ctz_builtin, true, uint8_t>::type>(str.size(), str.data(), delta, indices, lcp_values);
    }

  }

}


TEST(stacks, all_random) {
  const uint64_t runs = 1;
  for (uint64_t range = 2; range <= 1024 ; range <<= 1) {
    std::cout << "\n\nStarting tests with range " << range << std::endl;
    test_stacks(1ULL << 23, range, runs);
  }
  std::cout << "Done\n\n\n\n" << std::endl;
}

TEST(abort, abort) {
  EXPECT_EQ(true, false);
}
