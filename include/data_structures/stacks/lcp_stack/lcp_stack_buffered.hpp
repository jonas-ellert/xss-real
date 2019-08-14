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

#include <cmath>
#include <deque>
#include <util/common.hpp>

template <typename lcp_stack_type>
class lcp_stack_buffered {
private:
  const uint64_t buffer_size_;
  const uint64_t half_buffer_size_;

  lcp_stack_type lcp_stack_;

  std::deque<uint64_t> indices_;
  std::deque<uint64_t> lcps_;

  uint64_t size_ = 0;

  xssr_always_inline uint64_t get_max_size(const uint64_t n) {
    const uint64_t bytes = div<8>(n);
    const uint64_t words = div<8>(bytes);
    const uint64_t pairs = div<2>(words);
    return std::max(((pairs + 1) >> 1) << 1, (uint64_t) 65536); // at least 1MiB
  }

public:
  template <typename value_type>
  lcp_stack_buffered(const uint64_t n,
                     const uint64_t delta,
                     const value_type* text)
      : buffer_size_(get_max_size(n)),
        half_buffer_size_(buffer_size_ >> 1),
        lcp_stack_(n, delta, text) {
    indices_.push_front(0ULL);
    lcps_.push_front(0ULL);
  }

  xssr_always_inline uint64_t top_idx() const {
    return indices_.back();
  }

  xssr_always_inline uint64_t top_lcp() const {
    return lcps_.back();
  }

  xssr_always_inline void push_with_lcp(const uint64_t idx,
                                        const uint64_t lcp) {
    indices_.push_back(idx);
    lcps_.push_back(lcp);
    if (xssr_unlikely(indices_.size() == buffer_size_)) {
      if (xssr_unlikely(indices_.front() == 0)) {
        indices_.pop_front();
        lcps_.pop_front();
        ++size_;
      }
      for (uint64_t i = 0; i < half_buffer_size_; ++i) {
        lcp_stack_.push_with_lcp(indices_.front(), lcps_.front());
        indices_.pop_front();
        lcps_.pop_front();
      }
      size_ += half_buffer_size_;
    }
  }

  xssr_always_inline void push_without_lcp(const uint64_t idx) {
    indices_.push_back(idx);
  }

  xssr_always_inline void pop_with_lcp() {
    indices_.pop_back();
    lcps_.pop_back();
    if (xssr_unlikely(indices_.size() == 0)) {
      for (uint64_t i = 0; i < half_buffer_size_; ++i) {
        indices_.push_front(lcp_stack_.top_idx());
        lcps_.push_front(lcp_stack_.top_lcp());
        lcp_stack_.pop_with_lcp();
      }
      size_ -= half_buffer_size_;
      if (xssr_unlikely(lcp_stack_.top_idx() == 0)) {
        indices_.push_front(0);
        lcps_.push_front(0);
        --size_;
      }
    }
  }

  xssr_always_inline void pop_without_lcp() {
    indices_.pop_back();
  }

  xssr_always_inline uint64_t size() const {
    return size_ + indices_.size();
  }

  lcp_stack_buffered(const lcp_stack_buffered&) = delete;
  lcp_stack_buffered& operator=(const lcp_stack_buffered&) = delete;
};