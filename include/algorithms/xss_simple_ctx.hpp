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

template <typename bv_type>
class xss_simple_ctx {
private:
  constexpr static uint64_t lmask = 1ULL << 63;

  bv_type& bv_;
  uint64_t* data_;
  uint64_t current_word_size_;
  uint64_t current_word_data_index_;

  always_inline void automatic_new_word() {
    if (unlikely(current_word_size_ == 64)) {
      ++current_word_data_index_;
      current_word_size_ = 0;
    }
  }

public:
  xss_simple_ctx(bv_type& bv)
      : bv_(bv),
        data_(bv.data()),
        current_word_size_(0),
        current_word_data_index_(0) {}

  always_inline void open() {
    data_[current_word_data_index_] |= (lmask >> current_word_size_);
    current_word_size_++;
    automatic_new_word();
  }

  always_inline void close() {
    current_word_size_++;
    automatic_new_word();
  }

  always_inline bool operator[](uint64_t index) const {
    return (data_[div64(index)] & (lmask >> mod64(index)));
  }

  always_inline uint64_t current_length() const {
    return mul64(current_word_data_index_) + current_word_size_;
  }
};