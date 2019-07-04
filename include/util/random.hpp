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

#include <random>

using seed_type = std::random_device::result_type;

inline static seed_type get_random_seed() {
  static std::random_device rd;
  return rd();
}

template <typename T = uint64_t>
struct random_number_generator {

  random_number_generator(const T min, const T max, const seed_type seed)
      : seed_(seed), eng_(seed_), distr_(min, max) {}

  random_number_generator(const T min, const T max)
      : random_number_generator(min, max, get_random_seed()) {}
  random_number_generator(const seed_type seed)
      : random_number_generator(T_min, T_max, seed) {}
  random_number_generator()
      : random_number_generator(T_min, T_max, get_random_seed()) {}

  uint64_t operator()() {
    return distr_(eng_);
  }

  seed_type get_seed() const {
    return seed_;
  }

private:
  constexpr static uint64_t T_min = std::numeric_limits<T>::min();
  constexpr static uint64_t T_max = std::numeric_limits<T>::max();

  const seed_type seed_;
  std::mt19937 eng_;
  std::uniform_int_distribution<T> distr_;
};