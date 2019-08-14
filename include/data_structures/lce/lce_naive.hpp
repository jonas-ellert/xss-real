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

#include <util/common.hpp>

template <typename value_type = uint8_t>
class lce_naive {
public:
  struct lce {
    const value_type* text_;
    lce(const value_type* text) : text_(text) {}
    xssr_always_inline uint64_t operator()(const uint64_t i,
                                           const uint64_t j,
                                           uint64_t lcp = 0) const {
      while (text_[i + lcp] == text_[j + lcp])
        ++lcp;
      return lcp;
    }
  };

  struct suffix_compare {
    const value_type* text_;
    suffix_compare(const value_type* text) : text_(text) {}
    xssr_always_inline uint64_t operator()(const uint64_t i,
                                           const uint64_t j) const {
      uint64_t result = 0;
      while (text_[i + result] == text_[j + result])
        ++result;
      return text_[i + result] < text_[j + result];
    }
  };

  xssr_always_inline static lce get_lce(const value_type* text,
                                        [[maybe_unused]] const uint64_t n = 0) {
    return lce(text);
  }

  xssr_always_inline static suffix_compare
  get_suffix_compare(const value_type* text,
                     [[maybe_unused]] const uint64_t n = 0) {
    return suffix_compare(text);
  }

private:
  lce_naive() {}
};