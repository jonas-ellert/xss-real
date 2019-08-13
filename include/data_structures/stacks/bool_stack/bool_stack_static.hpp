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

class bool_stack_static {
private:
  const uint64_t data_size_;
  uint64_t* data_;
  uint64_t macro_idx_;
  uint64_t word_;
  uint64_t micro_idx_;

public:
  bool_stack_static(const uint64_t max_number_of_bits)
      : data_size_(div64((max_number_of_bits + 63))),
        data_(static_cast<uint64_t*>(malloc(mul8(data_size_)))),
        macro_idx_(0),
        word_(word_all_zero),
        micro_idx_(0) {
    // memset not necessary
  }

  ~bool_stack_static() {
    delete data_;
  }

  template <bool value>
  xssr_always_inline void push() {
    ++micro_idx_;
    if (xssr_unlikely(micro_idx_ == 64)) {
      data_[macro_idx_] = word_;
      ++macro_idx_;
      word_ = word_all_zero;
      micro_idx_ = 0;
    }

    if constexpr (value) {
      word_ |= word_left_one >> micro_idx_;
    } else {
      word_ &= ~(word_left_one >> micro_idx_);
    }
  }

  xssr_always_inline void push(bool value) {
    if (value)
      push<true>();
    else
      push<false>();
  }

  xssr_always_inline void push_true() {
    push<true>();
  }

  xssr_always_inline void push_false() {
    push<false>();
  }

  xssr_always_inline bool top() const {
    return (word_ & (word_left_one >> micro_idx_));
  }

  xssr_always_inline void pop() {
    if (xssr_unlikely(micro_idx_ == 0)) {
      word_ = data_[--macro_idx_];
      micro_idx_ = 64;
    }
    --micro_idx_;
  }
};
