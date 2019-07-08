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

#include <algorithms/psv_simple.hpp>
#include <data_structures/lce/lce_herlez.hpp>
#include <data_structures/lce/lce_herlez1k.hpp>

class xss_herlez {
public:
  template <typename value_type>
  static auto run(value_type* text, const uint64_t n) {
    auto compare = lce_herlez<value_type>::get_suffix_compare(text, n);
    return psv_simple<>::run_from_comparison(compare, n);
  }

  template <typename value_type>
  static auto run1k(value_type* text, const uint64_t n) {
    auto compare = lce_herlez1k<value_type>::get_suffix_compare(text, n);
    return psv_simple<>::run_from_comparison(compare, n);
  }
};