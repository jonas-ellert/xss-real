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
#include <data_structures/stacks/bool_stack/bool_stack.hpp>
#include <data_structures/stacks/unary_stack/unary_stack.hpp>
#include <util/common.hpp>

template <stack_strategy strategy, typename ctz_type, typename value_type>
class lcp_stack_delta_x {
private:
  constexpr static uint64_t minimum_n = 4096;
  const uint64_t n_;
  const uint64_t log2_delta_;
  const uint64_t delta_;

  const value_type* text_;

  telescope_stack<strategy, ctz_type> indices_;
  unary_stack<strategy, ctz_type> lcps_;
  uint64_t v_stack_size_;

  uint64_t top_lcp_;

  xssr_always_inline bool is_absolute_value(const uint64_t l1,
                                            const uint64_t l2) {
    return (l1 < l2 && delta_ <= l1);
  }

  xssr_always_inline bool is_relative_value(const uint64_t l1,
                                            const uint64_t l2) {
    return (l1 >= l2 && delta_ <= (l1 - l2));
  }

  xssr_always_inline bool is_transformable(const uint64_t l1,
                                           const uint64_t l2) {
    return is_absolute_value(l1, l2) || is_relative_value(l1, l2);
  }

public:
  lcp_stack_delta_x(const uint64_t n,
                    const uint64_t delta,
                    const value_type* text)
      : n_(n),
        log2_delta_((uint64_t) std::floor(std::log2(delta))),
        delta_(1ULL << log2_delta_),
        text_(text),
        indices_(n),
        lcps_((n >= minimum_n) ? ((4ULL * n) >> log2_delta_) : 128 * n),
        v_stack_size_(0),
        top_lcp_(0) {
    static_assert(strategy == STATIC || strategy == DYNAMIC);

    if (delta == 0) {
      std::cerr << "Delta cannot be 0." << std::endl;
      std::abort();
    }
  }

  xssr_always_inline void push_with_lcp(const uint64_t idx,
                                        const uint64_t lcp) {
    indices_.push(idx);
    if (is_absolute_value(top_lcp_, lcp)) {
      lcps_.push(top_lcp_ >> log2_delta_);
      ++v_stack_size_;
    } else if (is_relative_value(top_lcp_, lcp)) {
      lcps_.push((top_lcp_ - lcp) >> log2_delta_);
      ++v_stack_size_;
    }
    top_lcp_ = lcp;
  }

  xssr_always_inline void push_without_lcp(const uint64_t idx) {
    indices_.push(idx);
  }

  xssr_always_inline void pop_with_lcp() {
    indices_.pop();
    const uint64_t idx_2 = indices_.top();
    if (xssr_unlikely(idx_2 == 0)) {
      if (v_stack_size_ > 0) {
        lcps_.pop();
        --v_stack_size_;
      }
      top_lcp_ = 0;
      return;
    }

    indices_.pop();
    const uint64_t idx_1 = indices_.top();
    indices_.push(idx_2);

    const uint64_t transform = (v_stack_size_ > 0)
                                   ? (lcps_.top() << log2_delta_)
                                   : (top_lcp_ + delta_);
    uint64_t result = std::numeric_limits<uint64_t>::max();
    for (uint64_t i = 0; i < delta_; ++i) {
      if (text_[idx_1 + i] != text_[idx_2 + i]) {
        result = i;
        break;
      }
    }
    if (xssr_likely(idx_2 + top_lcp_ < n_))
      for (uint64_t i = 0; i < delta_; ++i) {
        if (text_[idx_1 + top_lcp_ + i] != text_[idx_2 + top_lcp_ + i]) {
          result = std::min((top_lcp_ + i), result);
          break;
        }
      }
    if (xssr_likely(idx_2 + transform < n_))
      for (uint64_t i = 0; i < delta_; ++i) {
        if (text_[idx_1 + transform + i] != text_[idx_2 + transform + i]) {
          result = std::min((transform + i), result);
          break;
        }
      }
    if (xssr_likely(idx_2 + top_lcp_ + transform < n_))
      for (uint64_t i = 0; i < delta_; ++i) {
        if (text_[idx_1 + transform + top_lcp_ + i] !=
            text_[idx_2 + transform + top_lcp_ + i]) {
          result = std::min((transform + top_lcp_ + i), result);
          break;
        }
      }

    if (is_transformable(result, top_lcp_)) {
      lcps_.pop();
      --v_stack_size_;
    }
    top_lcp_ = result;
  }

  xssr_always_inline void pop_without_lcp() {
    indices_.pop();
  }

  xssr_always_inline uint64_t top_idx() const {
    return indices_.top();
  }

  xssr_always_inline uint64_t top_lcp() const {
    return top_lcp_;
  }

  lcp_stack_delta_x(const lcp_stack_delta_x&) = delete;
  lcp_stack_delta_x& operator=(const lcp_stack_delta_x&) = delete;
};