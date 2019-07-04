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

static void ror_internal(uint8_t* result,
                         const uint64_t length,
                         const uint64_t repetitions,
                         const uint64_t char_idx) {

  if (length < repetitions) {
    for (uint64_t i = 0; i < repetitions; ++i) {
      result[i] = gen_alphabet[char_idx];
    }
    return;
  }

  uint64_t len_per_rep = length / repetitions;
  result[0] = gen_alphabet[char_idx];
  ror_internal(&(result[1]), len_per_rep - 1, repetitions, char_idx + 1);

  for (uint64_t i = len_per_rep; i < length; ++i) {
    result[i] = result[i - len_per_rep];
  }
}

[[maybe_unused]] static std::vector<uint8_t>
gen_run_of_runs(const uint64_t length, const uint64_t repetitions) {

  std::vector<uint8_t> result(length);
  ror_internal(result.data(), length, repetitions, 0);

  return result;
}