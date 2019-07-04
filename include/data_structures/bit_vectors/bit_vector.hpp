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

#include <cstring>
#include <sstream>
#include <util/common.hpp>

enum bit_vector_init { zero, one, uninitialized };

constexpr static bit_vector_init BV_FILL_ZERO = bit_vector_init::zero;
constexpr static bit_vector_init BV_FILL_ONE = bit_vector_init::one;
constexpr static bit_vector_init BV_UNINITIALIZED =
    bit_vector_init::uninitialized;

class bit_vector {
private:
  uint64_t n_;
  uint64_t data_size_;
  uint64_t* data_;

public:
  bit_vector(const uint64_t n, const bit_vector_init init)
      : n_(n),
        data_size_(div64((n_ + 63 + 64))),
        data_(static_cast<uint64_t*>(malloc(mul8(data_size_)))) {

    if (init == BV_FILL_ZERO)
      memset(data_, 0, mul8(data_size_));
    else if (init == BV_FILL_ONE)
      memset(data_, -1, mul8(data_size_));
  }

  ~bit_vector() {
    delete data_;
  }

  always_inline void set_one(const uint64_t idx) {
    data_[div64(idx)] |= (word_left_one >> (mod64(idx)));
  }

  always_inline void set_zero(const uint64_t idx) {
    data_[div64(idx)] &= ~(word_left_one >> (mod64(idx)));
  }

  template <bool value>
  always_inline void set(const uint64_t idx) {
    if constexpr (value)
      set_one(idx);
    else
      set_zero(idx);
  }

  always_inline void set(const uint64_t idx, const bool value) {
    if (value)
      set_one(idx);
    else
      set_zero(idx);
  }

  always_inline bool operator[](const uint64_t idx) const {
    return data_[div64(idx)] & (word_left_one >> (mod64(idx)));
  }

  always_inline bool get(const uint64_t idx) const {
    return operator[](idx);
  }

  always_inline uint64_t get_word(const uint64_t idx) const {
    const uint64_t bit_idx = mod64(idx);
    if (likely(bit_idx > 0))
      return (data_[div64(idx)] << bit_idx) |
             (data_[div64(idx) + 1] >> (64 - bit_idx));
    else
      return data_[div64(idx)];
  }

  always_inline void set_word(const uint64_t idx, const uint64_t word) const {
    const uint64_t bit_idx = mod64(idx);
    if (likely(bit_idx > 0)) {
      data_[div64(idx)] &= word_all_one << (64 - bit_idx);
      data_[div64(idx)] |= word >> bit_idx;
      data_[div64(idx) + 1] &= word_all_one >> bit_idx;
      data_[div64(idx) + 1] |= word << (64 - bit_idx);
    } else
      data_[div64(idx)] = word;
  }

  always_inline uint64_t size() const {
    return n_;
  }

  always_inline uint64_t* data() {
    return data_;
  }

  always_inline const uint64_t* data() const {
    return data_;
  }

  always_inline uint64_t data_size() const {
    return data_size_;
  }

  always_inline bool operator==(const bit_vector& other) const {
    if (n_ != other.n_)
      return false;
    for (uint64_t i = 0; i < data_size_; ++i) {
      if (unlikely(data_[i] != other.data_[i]))
        return false;
    }
    return true;
  }

  always_inline bool operator!=(const bit_vector& other) const {
    return !operator==(other);
  }

  template <bool ansi_color = true>
  std::string to_string(uint64_t len = 0) const {
    len = (len == 0) ? n_ : std::min(len, n_);

    const static std::string o = ansi_color ? "\033[1;32m(\033[0m" : "(";
    const static std::string c = ansi_color ? "\033[1;31m)\033[0m" : ")";
    const static std::string arrow = ansi_color ? "\033[1;34m->\033[0m" : "->";

    std::stringstream result;
    for (uint64_t i = 0; i < len; ++i) {
      if (operator[](i))
        result << o;
      else
        result << o;
    }
    if (len < n_)
      result << arrow;
    return result.str();
  }

  bit_vector& operator=(bit_vector&& other) {
    n_ = other.n_;
    data_size_ = other.data_size_;
    std::swap(data_, other.data_);
    return (*this);
  }

  bit_vector(bit_vector&& other) {
    (*this) = std::move(other);
  }

  bit_vector& operator=(const bit_vector& other) = delete;
  bit_vector(const bit_vector& other) = delete;
};
