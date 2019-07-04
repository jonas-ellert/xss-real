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

#include <data_structures/ctz/ctz.hpp>
#include <util/random.hpp>
#include <util/time_measure.hpp>

template <typename ctz_type>
static uint64_t bench_ctz(const std::vector<uint64_t>& queries,
                          const uint64_t r,
                          const int64_t tz) {

  std::cout << "RESULT algo=" << ctz_type::to_string() << " runs=" << r
            << " queries=" << queries.size() << " ctz_value=" << tz
            << std::flush;

  uint64_t y = 0;
  auto time_mem = get_time_mem(
      [&]() {
        for (uint64_t i = 0; i < queries.size(); ++i) {
          y += ctz_type::get_unsafe(queries[i]);
        }
      },
      r);

  std::cout << " median_time=" << time_mem.first
            << " qms=" << queries.size() / time_mem.first << std::endl;
  return y;
}

static void bench_ctz(const uint64_t queries, const uint64_t runs) {
  const uint64_t q = (queries == 0) ? (64ULL * 1024 * 1024) : queries;
  const uint64_t r = runs;

  std::cout << "Benchmarking CTZ: queries=" << q << ", runs=" << r << std::endl;

  auto rng = random_number_generator<uint64_t>();
  std::vector<uint64_t> qu(q, 0);

  for (uint64_t i = 0; i < q; ++i) {
    while (qu[i] == 0)
      qu[i] = rng();
  }
  std::cout << "Running completely random with x > 0..." << std::endl;

  uint64_t y = 0;
  y += bench_ctz<ctz_auto<CTZ_BUILTIN>::type>(qu, r, -1);
  y += bench_ctz<ctz_auto<CTZ_DEBRUIJN>::type>(qu, r, -1);
  y += bench_ctz<ctz_auto<CTZ_LOGARITHMIC>::type>(qu, r, -1);
  y += bench_ctz<ctz_auto<CTZ_LOGARITHMIC, 1>::type>(qu, r, -1);
  y += bench_ctz<ctz_auto<CTZ_LOGARITHMIC, 2>::type>(qu, r, -1);
  y += bench_ctz<ctz_auto<CTZ_LOGARITHMIC, 4>::type>(qu, r, -1);
  y += bench_ctz<ctz_auto<CTZ_LOGARITHMIC, 8>::type>(qu, r, -1);
  y += bench_ctz<ctz_auto<CTZ_LOGARITHMIC, 16>::type>(qu, r, -1);

  for (uint64_t i = 0; i < q; ++i) {
    qu[i] = rng() | 1ULL;
  }

  for (uint64_t tz = 0; tz < 64; ++tz) {
    std::cout << "Running queries with x=2^" << tz << "..." << std::endl;

    y += bench_ctz<ctz_auto<CTZ_BUILTIN>::type>(qu, r, tz);
    y += bench_ctz<ctz_auto<CTZ_DEBRUIJN>::type>(qu, r, tz);
    y += bench_ctz<ctz_auto<CTZ_LOGARITHMIC>::type>(qu, r, tz);
    y += bench_ctz<ctz_auto<CTZ_LOGARITHMIC, 1>::type>(qu, r, tz);
    y += bench_ctz<ctz_auto<CTZ_LOGARITHMIC, 2>::type>(qu, r, tz);
    y += bench_ctz<ctz_auto<CTZ_LOGARITHMIC, 4>::type>(qu, r, tz);
    y += bench_ctz<ctz_auto<CTZ_LOGARITHMIC, 8>::type>(qu, r, tz);
    y += bench_ctz<ctz_auto<CTZ_LOGARITHMIC, 16>::type>(qu, r, tz);
    //    y += bench_ctz<ctz_auto<CTZ_LINEAR>::type>(qu, r, tz);
    //    y += bench_ctz<ctz_auto<CTZ_LINEAR, 1>::type>(qu, r, tz);
    //    y += bench_ctz<ctz_auto<CTZ_LINEAR, 2>::type>(qu, r, tz);
    //    y += bench_ctz<ctz_auto<CTZ_LINEAR, 4>::type>(qu, r, tz);
    //    y += bench_ctz<ctz_auto<CTZ_LINEAR, 8>::type>(qu, r, tz);
    //    y += bench_ctz<ctz_auto<CTZ_LINEAR, 16>::type>(qu, r, tz);

    for (uint64_t i = 0; i < q; ++i) {
      qu[i] <<= 1;
    }
  }
  std::cout << "Done. (" << y << ")" << std::endl;
}
