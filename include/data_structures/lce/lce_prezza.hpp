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

#include <data_structures/lce/rk/rk_lce.hpp>
#include <util/common.hpp>

template <typename value_type = uint8_t>
class lce_prezza {
public:
  struct lce {
    rklce::rk_lce rk_lce_;
    lce(const value_type* text, const uint64_t n) : rk_lce_(text, n) {}
    xssr_always_inline uint64_t operator()(const uint64_t i, const uint64_t j) {
      return rk_lce_.LCE(i, j);
    }
  };

  struct suffix_compare {
    const value_type* text_;
    lce rk_lce_;
    suffix_compare(const value_type* text, const uint64_t n)
        : text_(text), rk_lce_(text, n) {}
    xssr_always_inline uint64_t operator()(const uint64_t i, const uint64_t j) {
      uint64_t lce = rk_lce_(i, j);
      return text_[i + lce] < text_[j + lce];
    }
  };

  xssr_always_inline static lce get_lce(const value_type* text,
                                        const uint64_t n) {
    return lce(text, n);
  }

  xssr_always_inline static suffix_compare
  get_suffix_compare(const value_type* text, const uint64_t n) {
    return suffix_compare(text, n);
  }

private:
  lce_prezza() {}
};