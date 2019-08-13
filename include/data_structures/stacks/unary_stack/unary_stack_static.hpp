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

#include <data_structures/bit_vectors/bit_vector.hpp>
#include <data_structures/bit_vectors/support/left_zeros.hpp>
#include <util/common.hpp>

template <typename ctz_type>
class unary_stack_static {
private:
  bit_vector bv_;
  left_zeros<ctz_type> count_left_zeros_;

  uint64_t* data_;
  uint64_t top_bit_;
  uint64_t top_value_;
  uint64_t jmp_idx_;

public:
  unary_stack_static(const uint64_t n)
      : bv_(n + 65, BV_FILL_ZERO),
        count_left_zeros_(bv_),
        data_(bv_.data()),
        top_bit_(0),
        top_value_(1),
        jmp_idx_(bv_.data_size() - 1) {

    // set first bit to 1 (always contain element 0 as sentinel)
    data_[0] = 1ULL << 63;
    // fill last word with 1s
    data_[jmp_idx_] = 0ULL - 1;
  }

  xssr_always_inline void push(const uint64_t value) {
    if (top_value_ > 127) {
      data_[--jmp_idx_] = top_value_;
      data_[--jmp_idx_] = top_bit_;
    } else {
      top_bit_ += top_value_;
      bv_.set_one(top_bit_);
    }
    top_value_ = value;
  }

  xssr_always_inline uint64_t top() const {
    return top_value_;
  }

  xssr_always_inline void pop() {
    if (top_bit_ == data_[jmp_idx_]) {
      top_value_ = data_[jmp_idx_ + 1];
      data_[jmp_idx_] = data_[jmp_idx_ + 1] = 0;
      jmp_idx_ += 2;
    } else {
      bv_.set_zero(top_bit_);
      const uint64_t offset = count_left_zeros_.get128(top_bit_) + 1;
      top_bit_ -= offset;
      top_value_ = offset;
    }
  }

  unary_stack_static(const unary_stack_static&) = delete;
  unary_stack_static& operator=(const unary_stack_static&) = delete;

  unary_stack_static& operator=(unary_stack_static&& other) {
    std::swap(bv_, other.bv_);
    count_left_zeros_ = other.count_left_zeros_;
    data_ = other.data_;
    top_bit_ = other.top_bit_;
    top_value_ = other.top_value_;
    jmp_idx_ = other.jmp_idx_;
    return *this;
  }

  unary_stack_static(unary_stack_static&& other) {
    (*this) = std::move(other);
  }
};
