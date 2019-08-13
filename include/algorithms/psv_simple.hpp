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

#include <algorithms/xss_simple_ctx.hpp>
#include <data_structures/bit_vectors/bit_vector.hpp>
#include <data_structures/stacks/naive_stack/naive_stack.hpp>
#include <data_structures/stacks/telescope_stack/telescope_stack.hpp>

template <stack_strategy strategy = NAIVE, typename ctz_type = ctz_builtin>
class psv_simple {
public:
  template <typename value_type>
  static auto run(const value_type* text, const uint64_t n) {
    struct compare_values {
      const value_type* text_;
      compare_values(const value_type* text) : text_(text) {}
      xssr_always_inline bool operator()(const uint64_t i, const uint64_t j) const {
        return (text_[i] <= text_[j]);
      }
    };
    compare_values compare(text);
    return run_from_comparison(compare, n);
  }

  template <typename comparison_type, bool is_64_bit = true>
  static auto run_from_comparison(comparison_type& compare, const uint64_t n) {

    using stack_type = telescope_stack<strategy, ctz_type>;

    bit_vector result(2 * n + 2, BV_FILL_ZERO);
    xss_simple_ctx<bit_vector> ctx(result);
    ctx.open();
    ctx.open();

    stack_type open_indices(n);
    open_indices.push(0);

    // 1 to n-2
    for (uint64_t i = 1; i < n - 1; ++i) {
      while (compare(i, open_indices.top())) {
        open_indices.pop();
        ctx.close();
      }
      open_indices.push(i);
      ctx.open();
    }

    while (open_indices.top() > 0) {
      open_indices.pop();
      ctx.close();
    }
    ctx.close();
    ctx.open();
    ctx.close();
    ctx.close();
    return result;
  }
};