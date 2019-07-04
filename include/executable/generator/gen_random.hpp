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

#include <executable/generator/alphabet.hpp>
#include <util/common.hpp>
#include <util/random.hpp>

static std::vector<uint8_t> gen_random(const uint64_t length,
                                       const uint64_t alphabet,
                                       const uint64_t seed) {
  const seed_type proper_seed = ((length % 1777) * seed) % 7177 + seed;
  const uint8_t min = 0;
  const uint8_t max = alphabet - 1;

  auto rng = random_number_generator(min, max, proper_seed);

  std::vector<uint8_t> result(length);
  for (uint64_t i = 0; i < length; ++i) {
    result[i] = gen_alphabet[rng()];
  }
  return result;
}