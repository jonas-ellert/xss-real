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

#include <data_structures/stacks/lcp_stack/lcp_stack_buffered.hpp>
#include <data_structures/stacks/lcp_stack/lcp_stack_delta.hpp>
#include <data_structures/stacks/lcp_stack/lcp_stack_naive.hpp>

template <stack_strategy strategy,
          typename ctz_type,
          bool use_delta_type,
          typename value_type>
class lcp_stack {
private:
  lcp_stack() {}

public:
  constexpr static stack_strategy strategy_unbuffered =
      (strategy == DYNAMIC_BUFFERED) ? DYNAMIC : strategy;

  using type_unbuffered =
      typename std::conditional<strategy_unbuffered == NAIVE,
                                lcp_stack_naive,
                                lcp_stack_delta<strategy_unbuffered,
                                                ctz_type,
                                                use_delta_type,
                                                value_type>>::type;

  using type = typename std::conditional<
      strategy == DYNAMIC_BUFFERED,
      lcp_stack_buffered<
          lcp_stack_delta<DYNAMIC, ctz_type, use_delta_type, value_type>>,
      type_unbuffered>::type;

  static type get_instance([[maybe_unused]] const value_type* text,
                           [[maybe_unused]] const uint8_t delta,
                           [[maybe_unused]] const uint64_t n) {
    return type(n, delta, text);
  }
};