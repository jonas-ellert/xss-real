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

#include <data_structures/bit_vectors/bit_vector.hpp>
#include <data_structures/lce/lce_naive.hpp>
#include <data_structures/stacks/lcp_stack/lcp_stack.hpp>
#include <util/random.hpp>
#include <util/time_measure.hpp>

template <typename stack_type>
static void bench_single_stack(const uint64_t n,
                               const uint8_t* text,
                               const uint64_t delta,
                               const std::vector<uint64_t>& indices,
                               const std::vector<uint64_t>& lcps,
                               const uint64_t runs,
                               const std::string info) {

  std::cout << "RESULT " << info << " "
            << "delta=" << delta << " "
            << "runs=" << runs << " "
            << "median_time=" << std::flush;

  stack_type stack(n, delta, text);

  const auto measure = get_time_mem(
      [&]() {
        for (uint64_t i = 0; i < indices.size(); ++i) {
          stack.push_with_lcp(indices[i], lcps[i]);
        }
        for (uint64_t i = 0; i < indices.size(); ++i) {
          stack.pop_with_lcp();
        }
      },
      runs);

  std::cout << measure.first << " memory=" << measure.second << std::endl;
}

static void
bench_stacks(const uint64_t n, const uint64_t range, const uint64_t runs) {

  random_number_generator<uint64_t> rngrange(0, range - 1);
  random_number_generator<uint64_t> rng2(0, 1);

  std::vector<uint64_t> lcp_values;
  lcp_values.reserve(n);
  for (uint64_t i = 0; i < n; ++i) {
    lcp_values.push_back(rngrange());
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

  // verify
  auto lce = lce_naive<uint8_t>::get_lce(str.data());
  for (uint64_t k = 1; k < lcp_values.size(); ++k) {
    if (lce(indices[k - 1], indices[k]) != lcp_values[k]) {
      std::cout << "Wrong LCE" << std::endl;
      return;
    }
  }

  std::cout << "Prepared data for range " << range << "." << std::endl;
  std::cout << "n = " << indices.size() << ", delta = " << 0 << std::endl;

  auto get_info = [&](const stack_strategy strategy) {
    return "stack_type=" + std::to_string(strategy) +
           " elements=" + std::to_string(indices.size()) +
           " range=" + std::to_string(range);
  };

  // plain types
  bench_single_stack<
      typename lcp_stack<NAIVE, ctz_builtin, false, uint8_t>::type>(
      str.size(), str.data(), 0, indices, lcp_values, runs, get_info(NAIVE));
  bench_single_stack<
      typename lcp_stack<STATIC, ctz_builtin, false, uint8_t>::type>(
      str.size(), str.data(), 0, indices, lcp_values, runs, get_info(STATIC));
  bench_single_stack<
      typename lcp_stack<DYNAMIC, ctz_builtin, false, uint8_t>::type>(
      str.size(), str.data(), 0, indices, lcp_values, runs, get_info(DYNAMIC));
  //  bench_single_stack<
  //      typename lcp_stack<DYNAMIC_BUFFERED, ctz_builtin, false,
  //      uint8_t>::type>( str.size(), str.data(), 0, indices, lcp_values, runs,
  //      get_info(DYNAMIC_BUFFERED));

  // delta types
  for (uint64_t delta = 1; delta <= 64; delta <<= 1) {
    std::cout << "n = " << indices.size() << ", delta = " << delta << std::endl;
    bench_single_stack<
        typename lcp_stack<STATIC, ctz_builtin, true, uint8_t>::type>(
        str.size(), str.data(), delta, indices, lcp_values, runs,
        get_info(STATIC));
    bench_single_stack<
        typename lcp_stack<DYNAMIC, ctz_builtin, true, uint8_t>::type>(
        str.size(), str.data(), delta, indices, lcp_values, runs,
        get_info(DYNAMIC));
    //    bench_single_stack<
    //        typename lcp_stack<DYNAMIC_BUFFERED, ctz_builtin, true,
    //        uint8_t>::type>( str.size(), str.data(), delta, indices,
    //        lcp_values, runs, get_info(DYNAMIC_BUFFERED));
  }
}

template <typename stack_type, typename vec_type>
static void bench_single_stack_only_unary(const vec_type& data,
                                          const uint64_t n,
                                          const std::string type,
                                          const uint64_t runs) {
  std::cout << "RESULT type=" << type << " "
            << "n=" << data.size() << " "
            << "runs=" << runs << " "
            << "median_time=" << std::flush;

  const auto measure = get_time_mem(
      [&]() {
        stack_type stack(n);
        for (uint64_t i = 0; i < data.size(); ++i) {
          stack.push(data[i]);
        }
        for (uint64_t i = 0; i < data.size(); ++i) {
          stack.pop();
        }
        // this is just here to prevent optimization
        if (stack.top() > 1)
          std::cout << stack.top() << std::endl;
      },
      //      [&]() {
      //        stack = stack_type(n);
      //      },
      runs);

  std::cout << measure.first << " "
            << "qpmicro=" << (data.size() / (double) measure.first) / 500
            << std::endl; //" memory=" << measure.second << std::endl;
}

[[maybe_unused]] static void bench_stacks_only_unary(const uint64_t n,
                                                     const uint64_t runs) {
  random_number_generator<uint64_t> rngrange(1, 127);
  random_number_generator<uint64_t> rng2(0, 1);
  std::vector<uint64_t> elements;

  uint64_t sum = 0;
  elements.resize(n);
  for (auto& el : elements) {
    el = rngrange();
    sum += el;
  }

  bench_single_stack_only_unary<naive_stack<uint64_t>>(
      elements, sum, "small stack=naive", runs);
  bench_single_stack_only_unary<unary_stack<STATIC, ctz_builtin>>(
      elements, sum, "small stack=static", runs);
  bench_single_stack_only_unary<unary_stack<DYNAMIC, ctz_builtin>>(
      elements, sum, "small stack=dynamic", runs);

  sum = 0;
  elements.resize(n);
  for (auto& el : elements) {
    el = 127 + rngrange() + rng2();
    sum += el;
  }

  bench_single_stack_only_unary<naive_stack<uint64_t>>(
      elements, sum, "large stack=naive", runs);
  bench_single_stack_only_unary<unary_stack<STATIC, ctz_builtin>>(
      elements, sum, "large stack=static", runs);
  bench_single_stack_only_unary<unary_stack<DYNAMIC, ctz_builtin>>(
      elements, sum, "large stack=dynamic", runs);

  sum = 0;
  elements.resize(n);
  for (auto& el : elements) {
    el = (rng2() ? (127 + rng2()) : 0) + rngrange();
    sum += el;
  }

  bench_single_stack_only_unary<naive_stack<uint64_t>>(
      elements, sum, "mixed stack=naive", runs);
  bench_single_stack_only_unary<unary_stack<STATIC, ctz_builtin>>(
      elements, sum, "mixed stack=static", runs);
  bench_single_stack_only_unary<unary_stack<DYNAMIC, ctz_builtin>>(
      elements, sum, "mixed stack=dynamic", runs);
}