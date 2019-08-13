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

#include <algorithms/duval.hpp>
#include <algorithms/xss_real_ctx.hpp>
#include <data_structures/bit_vectors/bit_vector.hpp>
#include <data_structures/lce/lce_naive.hpp>
#include <data_structures/stacks/telescope_stack/telescope_stack.hpp>
#include <sstream>
#include <stack>
#include <util/logging.hpp>

struct {
  uint64_t skipped_re = 0;
  uint64_t skipped_al = 0;
} xss_real_stats;

template <stack_strategy strategy = DYNAMIC_BUFFERED,
          typename ctz_type = ctz_builtin>
class xss_real {
public:
  template <bool stats = false, typename value_type>
  static auto
  run(const value_type* text, const uint64_t n, const uint64_t delta = 4) {
    return (delta > 0) ? run_internal<true, stats>(text, n, delta)
                       : run_internal<false, stats>(text, n, delta);
  }

private:
  template <bool use_delta_type, bool stats, typename value_type>
  static auto
  run_internal(const value_type* text, const uint64_t n, const uint64_t delta) {
    // provides naively computed LCP values
    const auto get_lcp = lce_naive<value_type>::get_lce(text);

    // active threshold must be at least 128
    // (this way run extension extends at least 64 bits of the bps)
    constexpr uint64_t active_threshold = 128;

    using ctx_type = xss_real_ctx<strategy, ctz_type, use_delta_type,
                                  bit_vector, value_type>;

    bit_vector result(2 * n + 2, BV_FILL_ZERO);
    ctx_type ctx(text, result, delta);
    ctx.open();
    ctx.open();

    if constexpr (stats) {
      xss_real_stats.skipped_al = 0;
      xss_real_stats.skipped_re = 0;
    }

    // 1 to n-2
    for (uint64_t i = 1; i < n - 1; ++i) {

      while (text[ctx.top_idx()] > text[i]) {
        ctx.pop_with_lcp();
        ctx.close();
      }

      uint64_t lcp = get_lcp(ctx.top_idx(), i, 0);
      uint64_t gamma = lcp;
      uint64_t j = ctx.top_idx();

      while (text[ctx.top_idx() + lcp] > text[i + lcp]) {
        uint64_t next_lcp = ctx.top_lcp();
        ctx.pop_with_lcp();
        ctx.close();
        // pop trivial
        while (next_lcp > lcp) {
          j = ctx.top_idx();
          next_lcp = ctx.top_lcp();
          ctx.pop_with_lcp();
          ctx.close();
        }

        if (xssr_unlikely(next_lcp == lcp)) {
          lcp = get_lcp(ctx.top_idx(), i, lcp);
          gamma = lcp;
          j = ctx.top_idx();
        } else {
          lcp = next_lcp;
          break;
        }
      }
      ctx.push_with_lcp(i, lcp);
      ctx.open();

      if (xssr_unlikely(gamma >= active_threshold)) {
        const uint64_t distance = i - j;
        bool suffix_j_smaller_i = text[j + gamma] < text[i + gamma];

        // EXTEND RUN -- EXTEND RUN -- EXTEND RUN -- EXTEND RUN -- EXTEND RUN --
        // END RUN -- EXTEND RUN -- EXTEND RUN -- EXTEND RUN -- EXTEND RUN -- EX
        if (xssr_unlikely(gamma >= 2 * distance)) {
          const uint64_t period = distance;
          const uint64_t repetitions = gamma / period - 1;
          // INCREASING RUN
          if (suffix_j_smaller_i) {
            ctx.extend_increasing_run(period, repetitions);
            for (uint64_t r = 0; r < repetitions; ++r) {
              i += period;
              gamma -= period;
              ctx.push_with_lcp(i, gamma);
            }
          }
          // DECREASING RUN
          else {
            ctx.extend_decreasing_run(period, repetitions);
            ctx.pop_without_lcp();
            i += period * repetitions;
            ctx.push_without_lcp(i);
          }
          if constexpr (stats) {
            xss_real_stats.skipped_re += period * repetitions;
          }
        }

        // AMORTIZE LOOKAHEAD -- AMORTIZE LOOKAHEAD -- AMORTIZE LOOKAHEAD -- AMO
        // RTIZE LOOKAHEAD -- AMORTIZE LOOKAHEAD -- AMORTIZE LOOKAHEAD -- AMORTI
        else {
          const uint64_t ell = div<4>(gamma);
          uint64_t anchor = ell;

          // check if gamm_ell is an extended lyndon run
          const auto gamma_str = &(text[i]);
          const auto duval =
              is_extended_lyndon_run(&(gamma_str[ell]), gamma - ell);

          // try to extend the lyndon run as far as possible to the left
          if (duval.first > 0) {
            const uint64_t period = duval.first;
            const auto repetition_eq = [&](const uint64_t l, const uint64_t r) {
              for (uint64_t k = 0; k < period; ++k)
                if (xssr_unlikely(gamma_str[l + k] != gamma_str[r + k]))
                  return false;
              return true;
            };
            int64_t lhs = ell + duval.second - period;
            while (lhs >= 0 && repetition_eq(lhs, lhs + period)) {
              lhs -= period;
            }
            anchor = std::min(ell, lhs + 2 * period);
          }

          // find the opening parenthesis of node j
          const uint64_t j_bps_idx = (ctx.current_length() - 1) - 2 * distance +
                                     ((suffix_j_smaller_i) ? 1 : 0);

          // copy (anchor - 1) opening parenthesis
          // and restore the stack H
          uint64_t last_text_idx = i;
          uint64_t last_bps_idx = j_bps_idx;
          while (last_text_idx < i + anchor - 1) {
            while (!ctx[++last_bps_idx]) {
              ctx.pop_without_lcp();
              ctx.close();
            }
            ++last_text_idx;
            ctx.push_without_lcp(last_text_idx);
            ctx.open();
          }

          // TODO: embedd in BPS
          // buffer all open indices on a stack (reverse the order)
          telescope_stack<strategy, ctz_type> buffer_reverse(anchor);
          const auto rev_transform = [&](const uint64_t idx) {
            return i + anchor - idx;
          };
          while (ctx.top_idx() > i) {
            buffer_reverse.push(rev_transform(ctx.top_idx()));
            ctx.pop_without_lcp();
          }

          const uint64_t rev_stop = rev_transform(0);
          const auto rev_top = [&]() {
            return rev_transform(buffer_reverse.top());
          };

          // restore stack L
          while (rev_top() != rev_stop) {
            uint64_t lcp = get_lcp(ctx.top_idx(), rev_top(), 0);
            uint64_t dist = rev_top() - ctx.top_idx();

            ctx.push_with_lcp(rev_top(), lcp);
            buffer_reverse.pop();

            while (lcp >= dist && rev_top() != rev_stop &&
                   ((rev_top() - ctx.top_idx()) == dist)) {
              lcp -= dist;
              ctx.push_with_lcp(rev_top(), lcp);
              buffer_reverse.pop();
            }
          }

          // continue with iteration i + anchor
          i += anchor - 1;

          if constexpr (stats) {
            xss_real_stats.skipped_al += anchor - 1;
          }
        }
      }
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