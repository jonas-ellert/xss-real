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

#include <lcePrezza.hpp>
#include <util/common.hpp>

template <typename value_type = uint8_t>
class lce_prezza {
public:
  template <typename vt>
  struct lce {
    LcePrezza prezza_;
    lce(vt* text, const uint64_t n)
        : prezza_(text, n) {}
    always_inline uint64_t operator()(const uint64_t i, const uint64_t j) {
      return prezza_.lce(i, j);
    }

    lce(const lce&) = delete;
    lce& operator=(const lce&) = delete;
  };

  template <typename vt>
  struct suffix_compare {
    LcePrezza prezza_;
    suffix_compare(vt* text, const uint64_t n)
        : prezza_(text, n) {}
    always_inline uint64_t operator()(const uint64_t i, const uint64_t j) {
      return prezza_.isSmallerSuffix(i, j);
    }

    suffix_compare(const suffix_compare&) = delete;
    suffix_compare& operator=(const suffix_compare&) = delete;
  };

  always_inline static lce<value_type> get_lce(value_type* text,
                                               const uint64_t n) {
    return lce(text, n);
  }

  always_inline static suffix_compare<value_type>
  get_suffix_compare(value_type* text, const uint64_t n) {
    return suffix_compare(text, n);
  }

private:
  lce_prezza() {}
};