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

static std::string to_SI_string(const uint64_t bytes) {
  const static std::string suffixes[] = {"B", "KiB", "MiB", "GiB", "TiB"};

  double bytes_dbl = bytes;
  uint64_t steps = 0;
  while (bytes_dbl > 1024) {
    bytes_dbl /= 1024;
    ++steps;
  }
  uint64_t result = std::round(bytes_dbl * 100);
  uint64_t pre = result / 100;
  uint64_t post = result % 100;

  return std::to_string(pre) + "." + ((post < 10) ? std::to_string(0) : "") +
         std::to_string(post) + suffixes[steps];
}
