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

#include <util/common.hpp>

template <typename stack_type>
class buffer_stack {
private:
  constexpr static uint64_t block_size_alloc = 512ULL * 1024;
  constexpr static uint64_t block_size_bits = 8ULL * block_size_alloc;
  constexpr static uint64_t block_size = block_size_alloc / sizeof(uint64_t);

  struct buffer_block {
    uint64_t* mem_;
    buffer_block* next_;
    buffer_block* prev_;

    buffer_block()
        : mem_(static_cast<uint64_t*>(malloc(block_size_alloc))),
          next_(nullptr),
          prev_(nullptr) {}

    ~buffer_block() {
      delete mem_;
    }
  };

  const uint64_t max_buffer_blocks_;
  const uint64_t move_blocks_;

  uint64_t current_buffer_blocks_;
  uint64_t current_data_blocks_;

  stack_type data_stack_;

  buffer_block* front_;
  buffer_block* back_;

  uint64_t* back_mem_;
  uint64_t back_idx_;

  xssr_always_inline uint64_t get_max_buffer_blocks(const uint64_t number_of_bits) {
    const uint64_t blocks =
        (number_of_bits + block_size_bits - 1) / block_size_bits;
    return (((blocks + 1) >> 1) << 1);
  }

public:
  template <typename... stack_arg_types>
  buffer_stack(const uint64_t number_of_bits, stack_arg_types&... stack_args)
      : max_buffer_blocks_(get_max_buffer_blocks(number_of_bits)),
        move_blocks_(max_buffer_blocks_ >> 1),
        current_buffer_blocks_(1),
        current_data_blocks_(0),
        data_stack_(stack_args...),
        front_(new buffer_block()),
        back_(front_),
        back_mem_(back_->mem_),
        back_idx_(0) {
    back_mem_[0] = 0; // always contain zero
  }

  ~buffer_stack() {
    while (front_ != nullptr) {
      auto prev_front = front_;
      front_ = front_->next_;
      delete prev_front;
    }
  }

  xssr_always_inline uint64_t top() const {
    return back_mem_[back_idx_];
  }

  xssr_always_inline void push(const uint64_t value) {
    if (xssr_unlikely(back_idx_ == block_size - 1)) {

      std::cout << "PUSH BLOCK" << std::endl;

      if (xssr_unlikely(current_buffer_blocks_ == max_buffer_blocks_)) {
        std::cout << "PUSH MOVE" << std::endl;

        const uint64_t first_i = ((current_data_blocks_ == 0) ? 1 : 0);
        uint64_t* current_mem = front_->mem_;
        for (uint64_t i = first_i; i < block_size; ++i) {
          data_stack_.push(current_mem[i]);
        }
        front_ = front_->next_;
        delete front_->prev_;
        for (uint64_t j = 1; j < move_blocks_; ++j) {
          current_mem = front_->mem_;
          for (uint64_t i = 0; i < block_size; ++i) {
            data_stack_.push(current_mem[i]);
          }
          front_ = front_->next_;
          delete front_->prev_;
        }
        current_buffer_blocks_ -= move_blocks_;
        current_data_blocks_ += move_blocks_;
      }

      if (back_->next_ == nullptr) {
        back_->next_ = new buffer_block();
        back_->next_->prev_ = back_;
      }
      back_ = back_->next_;
      back_mem_ = back_->mem_;
      back_idx_ = 0;
      back_mem_[0] = value;
      ++current_buffer_blocks_;
      return;
    }
    back_mem_[++back_idx_] = value;
  }

  xssr_always_inline void pop() {
    if (xssr_unlikely(back_idx_ == 0)) {

      std::cout << "POP BLOCK" << std::endl;

      if (xssr_unlikely(current_buffer_blocks_ == 1)) {

        std::cout << "POP MOVE " << current_data_blocks_ << std::endl;

        front_->prev_ = new buffer_block();
        front_->prev_->next_ = front_;
        front_ = front_->prev_;
        uint64_t* current_mem = front_->mem_;

        for (uint64_t j = 1; j < move_blocks_; ++j) {
          std::cout << "MOVE " << j << " ( " << move_blocks_ - 1 << std::endl;
          for (uint64_t i = block_size; i > 0;) {
            current_mem[--i] = data_stack_.top();
            data_stack_.pop();
          }
          front_->prev_ = new buffer_block();
          front_->prev_->next_ = front_;
          front_ = front_->prev_;
          current_mem = front_->mem_;
        }

        const uint64_t last_i =
            ((current_data_blocks_ == move_blocks_) ? 1 : 0);

        std::cout << "MOVE pl " << last_i << std::endl;

        for (uint64_t i = block_size; i > last_i;) {
          std::cout << i << std::flush;
          current_mem[--i] = data_stack_.top();
          std::cout << " " << current_mem[i] << std::flush;
          data_stack_.pop();
          std::cout << " done" << std::endl;
        }
        if (last_i == 1)
          current_mem[0] = 0;
        current_buffer_blocks_ += move_blocks_;
        current_data_blocks_ -= move_blocks_;

        std::cout << "POP MOVE DONE" << std::endl;
      }

      if (back_->next_ != nullptr) {
        delete back_->next_;
        back_->next_ = nullptr;
      }
      back_ = back_->prev_;
      back_mem_ = back_->mem_;
      back_idx_ = block_size - 1;
      --current_buffer_blocks_;
      return;
    }
    --back_idx_;
  }

  buffer_stack(const buffer_stack&) = delete;
  buffer_stack& operator=(const buffer_stack&) = delete;
};