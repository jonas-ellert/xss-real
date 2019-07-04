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

#include <cmath>
#include <executable/generator/alphabet.hpp>
#include <util/common.hpp>

static void gen_overlap_internal(const uint64_t n,
                                 const uint8_t min_char,
                                 std::vector<uint8_t>& result) {
  if (n == 5) {
    result.push_back(gen_alphabet[min_char]);
    result.push_back(gen_alphabet[min_char + 1]);
    result.push_back(gen_alphabet[min_char]);
    result.push_back(gen_alphabet[min_char + 1]);
    result.push_back(gen_alphabet[min_char]);
  } else {
    const uint64_t next_n = ((n + 1) / 3) - 1;
    result.push_back(gen_alphabet[min_char]);
    gen_overlap_internal(next_n, min_char + 1, result);
    result.push_back(gen_alphabet[min_char]);
    gen_overlap_internal(next_n, min_char + 1, result);
    result.push_back(gen_alphabet[min_char]);
    gen_overlap_internal(next_n, min_char + 1, result);
    result.pop_back();
  }
}

static std::vector<uint8_t> gen_overlap(const uint64_t n) {

  if (n < 5) {
    return std::vector<uint8_t>(n, gen_alphabet[0]);
  }

  const auto grow_n = [](const uint64_t v) { return 3 * v + 2; };

  uint64_t proper_n = 5;
  while (grow_n(proper_n) <= n) {
    proper_n = grow_n(proper_n);
  }

  std::vector<uint8_t> result;
  result.reserve(n);
  gen_overlap_internal(proper_n, 0, result);
  const auto len = result.size();
  while (result.size() < n)
    result.push_back(result[result.size() - len]);
  return result;
}