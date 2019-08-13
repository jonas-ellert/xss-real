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

#include <lcePrezza.hpp>
#include <util/common.hpp>

template <typename value_type = uint8_t>
class lce_herlez1k {
public:
  template <typename vt>
  struct lce {
    const vt* text_;
    vt* copy_;

    inline vt* get_copy(const vt* text, const uint64_t n) {
      vt* result = (vt*) malloc(sizeof(vt) * n);
      for (uint64_t i = 0; i < n; ++i) {
        result[i] = text[i];
      }
      return result;
    }

    LcePrezza prezza_;
    lce(const vt* text, const uint64_t n)
        : text_(text), copy_(get_copy(text, n)), prezza_(copy_, n) {}

    ~lce() {
      delete copy_;
    }

    xssr_always_inline uint64_t operator()(const uint64_t i, const uint64_t j) {
      uint64_t l = 0;
      while (text_[i + l] == text_[j + l] && xssr_likely(l < 1000))
        ++l;
      return xssr_likely(text_[i + l] != text_[j + l])
                 ? l
                 : (l + prezza_.lce(i + l, j + l));
    }

    lce(const lce&) = delete;
    lce& operator=(const lce&) = delete;
  };

  template <typename vt>
  struct suffix_compare {
    const vt* text_;
    lce<vt> prezzalce_;
    suffix_compare(const vt* text, const uint64_t n)
        : text_(text), prezzalce_(text, n) {}

    xssr_always_inline uint64_t operator()(const uint64_t i, const uint64_t j) {
      const uint64_t lce_result = prezzalce_(i, j);
      return text_[i + lce_result] < text_[j + lce_result];
    }

    suffix_compare(const suffix_compare&) = delete;
    suffix_compare& operator=(const suffix_compare&) = delete;
  };

  xssr_always_inline static lce<value_type> get_lce(const value_type* text,
                                               const uint64_t n) {
    return lce(text, n);
  }

  xssr_always_inline static suffix_compare<value_type>
  get_suffix_compare(const value_type* text, const uint64_t n) {
    return suffix_compare(text, n);
  }

private:
  lce_herlez1k() {}
};