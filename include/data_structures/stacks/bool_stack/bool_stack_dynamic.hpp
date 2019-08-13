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

#include <data_structures/stacks/naive_stack/naive_stack.hpp>
#include <util/common.hpp>

class bool_stack_dynamic {
private:
  naive_stack<uint64_t> data_;
  uint64_t micro_idx_;
  uint64_t word_;

public:
  bool_stack_dynamic() : micro_idx_(0), word_(word_all_zero) {}
  bool_stack_dynamic(const uint64_t) : bool_stack_dynamic() {}

  template <bool value>
  xssr_always_inline void push() {
    ++micro_idx_;
    if (xssr_unlikely(micro_idx_ == 64)) {
      data_.push(word_);
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

  xssr_always_inline void pop() {
    if (xssr_unlikely(micro_idx_ == 0)) {
      word_ = data_.top();
      data_.pop();
      micro_idx_ = 64;
    }
    --micro_idx_;
  }

  xssr_always_inline bool top() const {
    return (word_ & (word_left_one >> micro_idx_));
  }
};
