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

#include <sstream>
#include <random>
#include "executable/generator/gen_run_of_runs.hpp"

constexpr static uint8_t test_gen_sentinel = '\0';

using vec_type = std::vector<uint8_t>;

namespace std {
  [[maybe_unused]]
  static std::string to_string(const vec_type &vec) {
    std::stringstream stream;
    for (auto el : vec) {
      stream << uint8_t((el == test_gen_sentinel) ? '$' : el);
    }
    return stream.str();
  }

  [[maybe_unused]]
  static std::ostream& operator <<(std::ostream& stream, const vec_type &vec) {
    return stream << to_string(vec);
  }
}

[[maybe_unused]]
static vec_type generate_test_run_a(const uint64_t n) {
  vec_type result;
  result.reserve(n);
  result.push_back(test_gen_sentinel);
  while(result.size() < n - 1)
    result.push_back('A');
  result.push_back(test_gen_sentinel);
  return result;
}

static void generate_test_high_overlap_internal(const uint64_t n,
                                                const uint8_t min_char,
                                                vec_type &result) {
  if (n < 2) return;
  const auto pre_size = result.size();
  result.push_back(min_char);
  generate_test_high_overlap_internal((n - 2) / 2, min_char + 1, result);
  const auto len = result.size() - pre_size;
  for (uint64_t i = 0; i < len; ++i) {
    result.push_back(result[result.size() - len]);
  }
}

[[maybe_unused]]
static vec_type generate_test_high_overlap(const uint64_t n) {
  vec_type result;
  result.reserve(n);
  result.push_back(test_gen_sentinel);
  generate_test_high_overlap_internal(n - 2, 'A', result);
  const auto len = result.size() - 1;
  while (result.size() < n - 1)
    result.push_back(result[result.size() - len]);
  result.push_back(test_gen_sentinel);
  return result;
}

[[maybe_unused]]
static vec_type generate_test_ababc(uint64_t n) {
  const static std::string ababc = "ABABC";
  vec_type result;
  result.reserve(n);
  result.push_back(test_gen_sentinel);
  result.insert(result.end(), ababc.begin(), ababc.end());
  while (result.size() < n - 1)
    result.push_back(result[result.size() - 5]);
  result.push_back(test_gen_sentinel);
  return result;
}

static void generate_test_run_of_runs_internal(const uint64_t n, const uint64_t run_len, vec_type &result) {
  if (n < run_len + 1) {
    result.push_back('A');
    return;
  }
  const auto pre_size = result.size();
  generate_test_run_of_runs_internal((n - 1) / run_len, run_len, result);
  const auto len = result.size() - pre_size;
  for (uint64_t i = 0; i < run_len - 1; ++i) {
    for (uint64_t j = 0; j < len; ++j) {
      result.push_back(result[result.size() - len]);
    }
  }
  result.push_back(result[result.size() - 1] + 1);
}

[[maybe_unused]]
static vec_type generate_test_run_of_runs(uint64_t n, uint64_t run_len) {
//  vec_type result(n);
//  auto r = gen_run_of_runs(n - 2, run_len);
//  result[0] = result[n - 1] = test_gen_sentinel;
//  for (uint64_t i = 0; i < n - 2; ++i) {
//    result[i + 1] = r[i];
//  }
//  return result;
  vec_type result;
  result.reserve(n);
  result.push_back(test_gen_sentinel);
  generate_test_run_of_runs_internal(n - 2, run_len, result);
  const auto len = result.size() - 1;
  while (result.size() < n - 1)
    result.push_back(result[result.size() - len]);
  result.push_back(test_gen_sentinel);
  return result;
}

[[maybe_unused]]
static vec_type generate_test_random(const uint64_t n, const uint8_t sigma) {
  std::random_device rd;     // only used once to initialise (seed) engine
  std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
  std::uniform_int_distribution<unsigned int> uni(65, sigma + 64);
  auto nextRand = [&](){ return uint8_t(uni(rng)); };

  vec_type result;
  result.reserve(n);
  result.push_back(test_gen_sentinel);
  while(result.size() < n - 1)
    result.push_back(nextRand());
  result.push_back(test_gen_sentinel);
  return result;
}