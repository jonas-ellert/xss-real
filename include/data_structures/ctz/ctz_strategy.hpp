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

enum ctz_strategy : uint64_t {
  builtin = 0,
  linear = 1,
  logarithmic = 2,
  debruijn = 3
};

constexpr static ctz_strategy CTZ_BUILTIN = ctz_strategy::builtin;
constexpr static ctz_strategy CTZ_LINEAR = ctz_strategy::linear;
constexpr static ctz_strategy CTZ_LOGARITHMIC = ctz_strategy::logarithmic;
constexpr static ctz_strategy CTZ_DEBRUIJN = ctz_strategy::debruijn;

constexpr static ctz_strategy ctz_strategy_from_int(const uint64_t code) {
  if (code == 0)
    return ctz_strategy::builtin;
  if (code == 1)
    return ctz_strategy::linear;
  if (code == 2)
    return ctz_strategy::logarithmic;
  return ctz_strategy::debruijn;
}

constexpr static uint64_t ctz_type_to_int(const ctz_strategy strategy) {
  const uint64_t result = (uint64_t) strategy;
  return std::min(result, (uint64_t) 3);
}
