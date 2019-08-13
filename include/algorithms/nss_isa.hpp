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

#include <divsufsort.h>

class nss_isa {

public:
  template <typename value_type>
  static auto run(const value_type* text, const uint64_t n) {
    static_assert(sizeof(int) == 4);
    std::vector<int> result(n);

    int* sa = result.data();
    divsufsort((unsigned char*) (&(text[1])), sa, n - 1);

    int* isa = (int*) malloc(n * sizeof(int));
    isa[0] = 1;
    isa[n - 1] = 0;
    int* build_isa = &(isa[1]);
    for (uint64_t i = 1; i < n - 1; ++i) {
      build_isa[sa[i]] = i + 1;
    }

    int j, prev_j;
    int *nss = result.data();
    nss[0] = n - 1;
    nss[n - 1] = n;
    for (int i = 1; i < (int)n - 1; ++i) {
      j = i - 1;
      while (isa[j] > isa[i]) {
        prev_j = j;
        j = nss[prev_j];
        nss[prev_j] = i;
      }
      nss[i] = j;
    }

    j = n - 2;
    while (j > 0) {
      prev_j = j;
      j = nss[prev_j];
      nss[prev_j] = n - 1;
    }

    return result;
  }
};