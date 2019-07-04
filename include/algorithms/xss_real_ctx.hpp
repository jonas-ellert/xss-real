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

#include <bitset>
#include <data_structures/stacks/lcp_stack/lcp_stack.hpp>
#include <data_structures/stacks/stack_strategy.hpp>
#include <sstream>
#include <util/common.hpp>

#include <algorithms/xss_isa_psv.hpp>
#include <data_structures/bit_vectors/support/bps_support_sdsl.hpp>

template <stack_strategy strategy,
          typename ctz_type,
          bool use_delta_type,
          typename bv_type,
          typename value_type>
class xss_real_ctx {
private:
  using lcp_stack_type =
      typename lcp_stack<strategy, ctz_type, use_delta_type, value_type>::type;

  constexpr static uint64_t lmask = 1ULL << 63;

  const value_type* text_;
  const uint64_t n_;
  const uint64_t data_size_;
  uint64_t* data_;
  bv_type& bv_;

  lcp_stack_type lcp_stack_;

  uint64_t current_word_size_;
  uint64_t current_word_data_index_;

  always_inline void automatic_new_word() {
    if (unlikely(current_word_size_ == 64)) {
      ++current_word_data_index_;
      current_word_size_ = 0;
    }
  }

public:
  xss_real_ctx(const value_type* text, bv_type& bv, const uint64_t delta)
      : text_(text),
        n_(bv.size() / 2 - 1),
        data_size_(bv.data_size()),
        data_(bv.data()),
        bv_(bv),
        lcp_stack_(n_, delta, text),
        current_word_size_(0),
        current_word_data_index_(0) {}

  always_inline void push_with_lcp(const uint64_t idx, const uint64_t lcp) {
    lcp_stack_.push_with_lcp(idx, lcp);
  }

  always_inline void push_without_lcp(const uint64_t idx) {
    lcp_stack_.push_without_lcp(idx);
  }

  always_inline void pop_with_lcp() {
    lcp_stack_.pop_with_lcp();
  }

  always_inline void pop_without_lcp() {
    lcp_stack_.pop_without_lcp();
  }

  always_inline uint64_t top_idx() const {
    return lcp_stack_.top_idx();
  }

  always_inline uint64_t top_lcp() const {
    return lcp_stack_.top_lcp();
  }

  template <stack_strategy str = strategy>
  always_inline
      typename std::enable_if<str == strategy && strategy == DYNAMIC_BUFFERED,
                              uint64_t>::type
      size() const {
    return lcp_stack_.size();
  }

  always_inline void open() {
    data_[current_word_data_index_] |= (lmask >> current_word_size_);
    current_word_size_++;
    automatic_new_word();
  }

  always_inline void close() {
    current_word_size_++;
    automatic_new_word();
  }

  always_inline void soft_set_word(const uint64_t idx,
                                   const uint64_t word) const {
    const uint64_t bit_idx = mod64(idx);
    if (likely(bit_idx > 0)) {
      data_[div64(idx)] |= word >> bit_idx;
      data_[div64(idx) + 1] |= word << (64 - bit_idx);
    } else
      data_[div64(idx)] = word;
  }

  always_inline void append_copy_unsafe(const uint64_t source,
                                        const uint64_t length) {
    uint64_t dest = current_length();
    for (uint64_t i = 0; i < length; i += 64) {
      soft_set_word(dest + i, bv_.get_word(source + i));
    }
    const uint64_t cur_len = dest + length;
    current_word_data_index_ = div64(cur_len);
    current_word_size_ = mod64(cur_len);
    bv_.set_word(cur_len, word_all_zero);
  }

  // length must be at least 64!
  always_inline void append_copy(const uint64_t source, const uint64_t length) {
    uint64_t dest = current_length();
    uint64_t distance = dest - source;
    uint64_t start_word = bv_.get_word(source);
    if (distance < 64) {
      const uint64_t times = 64 / distance;
      const uint64_t tiny_length = times * distance;
      for (uint64_t i = 1; i < times; ++i) {
        start_word |= start_word >> distance;
      }
      soft_set_word(dest, start_word);
      current_word_data_index_ = div64(dest + tiny_length);
      current_word_size_ = mod64(dest + tiny_length);
      append_copy_unsafe(source, length - tiny_length);
    } else {
      append_copy_unsafe(source, length);
    }
  }

  always_inline void extend_increasing_run(const uint64_t period,
                                           const uint64_t repetitions) {
    const uint64_t copy_length_per_repetition = 2 * period - 1;
    const uint64_t copy_length_total = repetitions * copy_length_per_repetition;
    const uint64_t copy_from = current_length() - copy_length_per_repetition;
    append_copy(copy_from, copy_length_total);
  }

  always_inline void extend_decreasing_run(const uint64_t period,
                                           const uint64_t repetitions) {
    const uint64_t copy_length_per_repetition = 2 * period;
    const uint64_t copy_length_total = repetitions * copy_length_per_repetition;
    const uint64_t copy_from = current_length() - copy_length_per_repetition;
    append_copy(copy_from, copy_length_total);
  }

  always_inline bool operator[](uint64_t index) const {
    return (data_[div64(index)] & (lmask >> mod64(index)));
  }

  always_inline uint64_t current_length() const {
    return mul64(current_word_data_index_) + current_word_size_;
  }
};