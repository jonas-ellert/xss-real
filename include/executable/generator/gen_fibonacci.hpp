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

#include <util/common.hpp>

static std::vector<uint8_t> gen_fibonacci(const uint64_t length,
                                          const std::string fib0,
                                          const std::string fib1) {

  std::vector<std::string> fib(100);

  std::vector<uint8_t> result(length);

  fib[0] = fib0;
  fib[1] = fib1;
  uint64_t i;
  for (i = 2; i < 100; ++i) {
    fib[i] = fib[i - 2] + fib[i - 1];
    fib[i - 2] = "";
    if (fib[i].size() >= length) {
      break;
    }
  }

  const auto& fibres = fib[i];
  for (i = 0; i < length; ++i) {
    result[i] = fibres[i];
  }
  return result;
}