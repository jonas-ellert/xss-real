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

#include <data_structures/ctz/ctz.hpp>
#include <data_structures/stacks/naive_stack/naive_stack.hpp>
#include <stack>
#include <util/common.hpp>

template <typename ctz_type>
class unary_stack_dynamic {
private:
  naive_stack<uint64_t> data_left_;
  naive_stack<uint64_t> data_right_;

  uint64_t top_bit_;
  uint64_t top_bit_mod64_;
  uint64_t top_value_;

  uint64_t top_word_;

public:
  unary_stack_dynamic()
      : top_bit_(0),
        top_bit_mod64_(0),
        top_value_(1),
        top_word_(word_left_one) {
    // fill last word with 1s
    data_right_.push(word_all_one);
  }

  unary_stack_dynamic(const uint64_t) : unary_stack_dynamic(){};

  always_inline void push(const uint64_t value) {
    if (top_value_ > 127) {
      data_right_.push(top_value_);
      data_right_.push(top_bit_);
    } else {
      top_bit_ += top_value_;
      top_bit_mod64_ += top_value_;
      while (top_bit_mod64_ > 63) {
        top_bit_mod64_ -= 64;
        data_left_.push(top_word_);
        top_word_ = word_all_zero;
      }
      top_word_ |= word_left_one >> top_bit_mod64_;
    }
    top_value_ = value;
  }

  always_inline uint64_t top() const {
    return top_value_;
  }

  always_inline void pop() {
    if (top_bit_ == data_right_.top()) {
      data_right_.pop();
      top_value_ = data_right_.top();
      data_right_.pop();
    } else {
      const uint64_t previous_top_bit_ = top_bit_;
      top_word_ &= ~(word_left_one >> top_bit_mod64_);
      while (top_word_ == word_all_zero) {
        top_word_ = data_left_.top();
        data_left_.pop();
      }
      top_bit_mod64_ = 63 - ctz_type::get_unsafe(top_word_);
      top_bit_ = mul64(data_left_.size() - 1) + top_bit_mod64_;

      top_value_ = previous_top_bit_ - top_bit_;
    }
  }

  unary_stack_dynamic(const unary_stack_dynamic&) = delete;
  unary_stack_dynamic& operator=(const unary_stack_dynamic&) = delete;

  unary_stack_dynamic& operator=(unary_stack_dynamic&& other) {
    std::swap(data_left_, other.data_left_);
    std::swap(data_right_, other.data_right_);
    top_bit_ = other.top_bit_;
    top_bit_mod64_ = other.top_bit_mod64_;
    top_value_ = other.top_value_;
    top_word_ = other.top_word_;
    return *this;
  }

  unary_stack_dynamic(unary_stack_dynamic&& other) {
    (*this) = std::move(other);
  }
};
