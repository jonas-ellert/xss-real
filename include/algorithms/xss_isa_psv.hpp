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

#include <algorithms/xss_bps.hpp>
#include <divsufsort.h>

class xss_isa_psv {

public:
  template <typename value_type>
  static auto run(const value_type* text, const uint64_t n) {
    static_assert(sizeof(int) == 4);

    const uint64_t sa_bytes = (n - 1) * sizeof(int);
    const uint64_t isa_bytes = n * sizeof(int);
    int* sa = (int*) malloc(sa_bytes);
    divsufsort((unsigned char*) (&(text[1])), sa, n - 1);

    int* isa = (int*) malloc(isa_bytes);
    int* build_isa = &(isa[1]);
    for (uint64_t i = 0; i < n - 1; ++i) {
      build_isa[sa[i]] = i;
    }
    isa[0] = std::numeric_limits<int>::min();
    delete sa;
    auto result = psv_simple<>::run(isa, n);
    delete isa;
    return result;
  }
};