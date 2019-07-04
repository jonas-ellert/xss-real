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

#include <data_structures/bit_vectors/bit_vector.hpp>

template <typename ctz_type>
class left_zeros {
private:
  const uint64_t* data_;

  always_inline uint64_t get_left_word(const uint64_t idx) const {
    const uint64_t bit_idx = mod64(idx);
    if (likely(bit_idx > 0))
      return (data_[div64(idx)] >> (64 - bit_idx)) |
             (data_[div64(idx) - 1] << (bit_idx));
    else
      return data_[div64(idx) - 1];
  }

public:
  left_zeros(const bit_vector& bv) : data_(bv.data()) {}

  //  always_inline uint64_t operator()(uint64_t idx) const {
  //    uint64_t result = 0;
  //    uint64_t word;
  //    while ((word = get_left_word(idx)) == 0) {
  //      result += 64;
  //      idx -= 64;
  //    }
  //    return result + ctz_type::get(word);
  //  }

  always_inline uint64_t get128(uint64_t idx) const {
    const uint64_t word = get_left_word(idx);
    return (word > 0) ? ctz_type::get_unsafe(word)
                      : (64 + ctz_type::get_unsafe(get_left_word(idx - 64)));
  }
};