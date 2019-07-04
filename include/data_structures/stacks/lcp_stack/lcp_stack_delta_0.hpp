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

#include <data_structures/stacks/bool_stack/bool_stack.hpp>
#include <data_structures/stacks/telescope_stack/telescope_stack.hpp>
#include <data_structures/stacks/unary_stack/unary_stack.hpp>
#include <util/common.hpp>

template <stack_strategy strategy, typename ctz_type>
class lcp_stack_delta_0 {
private:
  constexpr static uint64_t minimum_n = 4096;
  telescope_stack<strategy, ctz_type> indices_;
  unary_stack<strategy, ctz_type> lcps_;
  bool_stack<strategy> type_stack_;
  uint64_t top_lcp_;

public:
  template <typename... dummy_types>
  lcp_stack_delta_0(const uint64_t n, const dummy_types&...)
      : indices_(n),
        lcps_((n >= minimum_n) ? (5ULL * n) : 128 * n),
        type_stack_(n),
        top_lcp_(0) {
    static_assert(strategy == STATIC || strategy == DYNAMIC);
  }

  always_inline void push_with_lcp(const uint64_t idx, const uint64_t lcp) {
    indices_.push(idx);
    // add +1 to allow zeros
    if (lcp <= top_lcp_) {
      lcps_.push(top_lcp_ - lcp + 1);
      type_stack_.push_true();
    } else {
      lcps_.push(top_lcp_ + 1);
      type_stack_.push_false();
    }
    top_lcp_ = lcp;
  }

  always_inline void push_without_lcp(const uint64_t idx) {
    indices_.push(idx);
  }

  always_inline void pop_with_lcp() {
    indices_.pop();
    if (type_stack_.top()) {
      top_lcp_ += lcps_.top() - 1;
    } else {
      top_lcp_ = lcps_.top() - 1;
    }
    lcps_.pop();
    type_stack_.pop();
  }

  always_inline void pop_without_lcp() {
    indices_.pop();
  }

  always_inline uint64_t top_idx() const {
    return indices_.top();
  }

  always_inline uint64_t top_lcp() const {
    return top_lcp_;
  }

  lcp_stack_delta_0(const lcp_stack_delta_0&) = delete;
  lcp_stack_delta_0& operator=(const lcp_stack_delta_0&) = delete;
};