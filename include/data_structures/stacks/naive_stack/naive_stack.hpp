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

#include <stack>
#include <util/common.hpp>

// at most 1MiB of memory overhead
template <typename value_type>
class naive_stack_custom {
private:
  constexpr static uint64_t block_size =
      512ULL * 1024ULL / sizeof(value_type); // 512KiB per block
  constexpr static uint64_t block_size_bytes = block_size * sizeof(value_type);

  struct block_manager {
    value_type* next_block = nullptr;

    always_inline value_type* new_block() {
      value_type* result = next_block;
      next_block = nullptr;
      return (result != nullptr)
                 ? result
                 : static_cast<value_type*>(malloc(block_size_bytes));
    }

    always_inline void delete_block(value_type* block) {
      delete next_block;
      next_block = block;
    }

    ~block_manager() {
      delete next_block;
    }
  };

  block_manager bmgr;
  std::deque<value_type*> blocks;
  value_type* top_block = bmgr.new_block();
  uint64_t top_idx = 0;
  uint64_t big_size = 0;

public:
  template <typename... arg_types>
  naive_stack_custom(const arg_types&...) {
    top_block[top_idx] = 0ULL; // always contains 0;
  }

  always_inline value_type top() const {
    return top_block[top_idx];
  }

  always_inline void pop() {
    if (unlikely(top_idx == 0)) {
      top_idx = block_size;
      bmgr.delete_block(top_block);
      top_block = blocks.back();
      blocks.pop_back();
      big_size -= block_size;
    }
    --top_idx;
  }

  always_inline void push(value_type value) {
    ++top_idx;
    if (unlikely(top_idx == block_size)) {
      top_idx = 0;
      blocks.push_back(top_block);
      top_block = bmgr.new_block();
      big_size += block_size;
    }
    top_block[top_idx] = value;
  }

  ~naive_stack_custom() {
    delete top_block;
    for (auto block : blocks) {
      delete block;
    }
  }

  always_inline uint64_t size() const {
    return top_idx + big_size + 1;
  }

  naive_stack_custom(const naive_stack_custom&) = delete;
  naive_stack_custom& operator=(const naive_stack_custom&) = delete;
};

template <typename value_type>
class naive_stack_std {
private:
  std::stack<value_type> data_;

public:
  template <typename... arg_types>
  naive_stack_std(const arg_types&...) {
    data_.push(word_all_zero);
  }

  always_inline value_type top() const {
    return data_.top();
  }

  always_inline void pop() {
    data_.pop();
  }

  always_inline void push(value_type value) {
    data_.push(value);
  }

  always_inline uint64_t size() const {
    return data_.size();
  }

  naive_stack_std(const naive_stack_std&) = delete;
  naive_stack_std& operator=(const naive_stack_std&) = delete;

  naive_stack_std& operator=(naive_stack_std&& other) {
    std::swap(data_, other.data_);
    return *this;
  }

  naive_stack_std(naive_stack_std&& other) {
    (*this) = std::move(other);
  }
};

template <typename value_type>
using naive_stack = naive_stack_std<value_type>;