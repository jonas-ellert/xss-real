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

#include <algorithms/xss_real_ctx.hpp>
#include <data_structures/bit_vectors/bit_vector.hpp>
#include <data_structures/lce/lce_naive.hpp>
#include <sstream>
#include <stack>
#include <util/logging.hpp>

template <stack_strategy strategy = DYNAMIC_BUFFERED,
          typename ctz_type = ctz_builtin>
class xss_bps_lcp {
public:
  template <typename value_type>
  static auto
  run(const value_type* text, const uint64_t n, const uint64_t delta = 4) {
    return (delta > 0) ? run_internal<true>(text, n, delta)
                       : run_internal<false>(text, n, delta);
  }

private:
  template <bool use_delta_type, typename value_type>
  static auto
  run_internal(const value_type* text, const uint64_t n, const uint64_t delta) {
    // provides naively computed LCP values
    const auto get_lcp = lce_naive<value_type>::get_lce(text);

    using ctx_type = xss_real_ctx<strategy, ctz_type, use_delta_type,
                                  bit_vector, value_type>;

    bit_vector result(2 * n + 2, BV_FILL_ZERO);
    ctx_type ctx(text, result, delta);

    ctx.open();
    ctx.open();

    // 1 to n-2
    for (uint64_t i = 1; i < n - 1; ++i) {
      while (text[ctx.top_idx()] > text[i]) {
        ctx.pop_with_lcp();
        ctx.close();
      }

      uint64_t lcp = get_lcp(ctx.top_idx(), i, 0);
      while (text[ctx.top_idx() + lcp] > text[i + lcp]) {
        uint64_t next_lcp = ctx.top_lcp();
        ctx.pop_with_lcp();
        ctx.close();
        // pop trivial
        while (next_lcp > lcp) {
          next_lcp = ctx.top_lcp();
          ctx.pop_with_lcp();
          ctx.close();
        }

        if (unlikely(next_lcp == lcp)) {
          lcp = get_lcp(ctx.top_idx(), i, lcp);
        } else {
          lcp = next_lcp;
          break;
        }
      }
      ctx.push_with_lcp(i, lcp);
      ctx.open();
    }

    if constexpr (strategy == DYNAMIC_BUFFERED) {
      const uint64_t not_closed = ctx.size();
      for (uint64_t i = 1; i < not_closed; ++i) {
        ctx.close();
      }
    } else {
      while (ctx.top_idx() > 0) {
        ctx.close();
        ctx.pop_without_lcp();
      }
    }
    ctx.close();
    ctx.open();
    ctx.close();
    ctx.close();
    return result;
  }
};