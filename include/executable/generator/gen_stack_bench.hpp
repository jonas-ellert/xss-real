//  Copyright (c) 2019 Jonas Ellert
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to
//  deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
//  sell copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//  IN THE SOFTWARE.

#pragma once

#include <cmath>
#include <executable/generator/alphabet.hpp>
#include <util/common.hpp>

static std::vector<uint8_t> gen_stack_bench_base(const uint64_t period) {
  std::vector<uint8_t> result()
}

static std::vector<uint8_t> gen_stack_bench(const uint64_t length,
                                            const uint64_t period) {
  auto init = [length](std::vector<uint8_t>& result, const uint64_t period) {
    for (uint64_t i = 0; i < std::min(period, length) - 1; ++i) {
      result[i] = gen_alphabet[0];
    }
    if (length >= period)
      result[period - 1] = gen_alphabet[1];
  };
  return r_internal(length, period, init);
}