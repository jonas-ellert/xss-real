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
#include <stack>

class lcp_stack_naive {

private:
  naive_stack<uint64_t> indices_;
  naive_stack<uint64_t> lcps_;

  //  std::stack<uint64_t> indices_;
  //  std::stack<uint64_t> lcps_;

public:
  template <typename... dummy_types>
  lcp_stack_naive(const dummy_types&...) {
    //    indices_.push(0);
    //    lcps_.push(0);
  }

  always_inline void push_with_lcp(const uint64_t idx, const uint64_t lcp) {
    indices_.push(idx);
    lcps_.push(lcp);
  }

  always_inline void push_without_lcp(const uint64_t idx) {
    indices_.push(idx);
  }

  always_inline void pop_with_lcp() {
    indices_.pop();
    lcps_.pop();
  }

  always_inline void pop_without_lcp() {
    indices_.pop();
  }

  always_inline uint64_t top_idx() const {
    return indices_.top();
  }

  always_inline uint64_t top_lcp() const {
    return lcps_.top();
  }

  lcp_stack_naive(const lcp_stack_naive&) = delete;
  lcp_stack_naive& operator=(const lcp_stack_naive&) = delete;
};