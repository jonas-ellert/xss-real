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

#include <algorithm>
#include <chrono>
#include <sstream>
#ifdef MALLOC_COUNT
#include <malloc_count.h>
#endif // MALLOC_COUNT

struct time_measure {
  decltype(std::chrono::high_resolution_clock::now()) begin_;
  decltype(std::chrono::high_resolution_clock::now()) end_;

  void begin() {
    begin_ = std::chrono::high_resolution_clock::now();
  }

  void end() {
    end_ = std::chrono::high_resolution_clock::now();
  }

  uint64_t millis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end_ - begin_)
        .count();
  }
};

template <typename func_type, typename post_type>
static inline std::pair<uint64_t, uint64_t> get_time_mem(const func_type& func,
                                                         const post_type& post,
                                                         const uint64_t runs) {
  std::vector<uint64_t> times(runs);

#ifdef MALLOC_COUNT
  std::vector<std::pair<uint64_t, uint64_t>> mem_times(runs);
#endif // MALLOC_COUNT

  for (uint64_t i = 0; i < runs; ++i) {
    time_measure time_measurement;

#ifdef MALLOC_COUNT
    malloc_count_reset_peak();
    uint64_t mem_pre = malloc_count_current();
#endif // MALLOC_COUNT

    time_measurement.begin();
    func();
    time_measurement.end();
    post();
    uint64_t time = time_measurement.millis();
    times[i] = time;

#ifdef MALLOC_COUNT
    uint64_t mem_peak = malloc_count_peak();
    uint64_t memory = mem_peak - mem_pre;
    mem_times[i] = {memory, time};
#endif // MALLOC_COUNT
  }
  std::sort(times.begin(), times.end());
  const uint64_t median_time = std::max((uint64_t) 1, times[runs >> 1]);

#ifdef MALLOC_COUNT
  uint64_t median_mem = std::numeric_limits<uint64_t>::max();
  for (const auto& mem_time : mem_times) {
    if (mem_time.second == median_time && mem_time.first < median_mem)
      median_mem = mem_time.first;
  }
  return {median_time, median_mem};
#else
  return {median_time, 0};
#endif // MALLOC_COUNT
}

template <typename func_type>
static inline std::pair<uint64_t, uint64_t> get_time_mem(const func_type& func,
                                                         const uint64_t runs) {
  return get_time_mem(func, []() {}, runs);
}