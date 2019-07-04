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

#include <cmath>
#include <data_structures/stacks/telescope_stack/telescope_stack_dynamic.hpp>
#include <deque>
#include <util/common.hpp>

template <typename ctz_type>
class telescope_stack_buffered {
private:
  const uint64_t buffer_size_;
  const uint64_t half_buffer_size_;

  telescope_stack_dynamic<ctz_type> tele_stack_;
  std::deque<uint64_t> elements_;

  always_inline uint64_t get_max_size(const uint64_t n) {
    const uint64_t bytes = div<8>(n);
    const uint64_t words = div<8>(bytes);
    return std::max(((words + 1) >> 1) << 1, (uint64_t) 65536); // at least 1MiB
  }

public:
  telescope_stack_buffered(const uint64_t n)
      : buffer_size_(get_max_size(n)), half_buffer_size_(buffer_size_ >> 1) {
    elements_.push_front(0ULL);
  }

  always_inline uint64_t top() const {
    return elements_.back();
  }

  always_inline void push(const uint64_t e) {
    elements_.push_back(e);
    if (unlikely(elements_.size() == buffer_size_)) {
      if (unlikely(elements_.front() == 0)) {
        elements_.pop_front();
      }
      for (uint64_t i = 0; i < half_buffer_size_; ++i) {
        tele_stack_.push(elements_.front());
        elements_.pop_front();
      }
    }
  }

  always_inline void pop() {
    elements_.pop_back();
    if (unlikely(elements_.size() == 0)) {
      for (uint64_t i = 0; i < half_buffer_size_; ++i) {
        elements_.push_front(tele_stack_.top());
        tele_stack_.pop();
      }
      if (unlikely(tele_stack_.top() == 0)) {
        elements_.push_front(0);
      }
    }
  }

  telescope_stack_buffered(const telescope_stack_buffered&) = delete;
  telescope_stack_buffered& operator=(const telescope_stack_buffered&) = delete;
};