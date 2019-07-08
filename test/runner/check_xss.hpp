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

#include "util/enums.hpp"
#include <algorithms/xss_real.hpp>
#include <algorithms/xss_bps.hpp>
#include <algorithms/xss_bps_lcp.hpp>
#include <algorithms/psv_simple.hpp>
#include <algorithms/xss_herlez.hpp>
#include <algorithms/xss_isa_psv.hpp>
#include <data_structures/stacks/stack_strategy.hpp>
#include <data_structures/lce/lce_prezza.hpp>

template <stack_strategy strategy, typename check_type, typename vec_type, typename result_type>
static void check_all_xss_algos(const vec_type &instance, const result_type &correct_result) {
  constexpr uint64_t max_delta = (strategy != NAIVE) ? 32 : 1;
  for (uint64_t delta = 1; delta <= max_delta; delta = delta << 1) {
    auto res = xss_real<strategy, ctz_builtin>::run(instance.data(), instance.size(), delta);
    if (res != correct_result)
      check_type::check(instance, res);
    res = xss_bps_lcp<strategy, ctz_builtin>::run(instance.data(), instance.size(), delta);
    if (res != correct_result)
      check_type::check(instance, res);
  }

  auto res = xss_bps<strategy, ctz_builtin>::run(instance.data(), instance.size());
  if (res != correct_result)
    check_type::check(instance, res);
}

template <typename check_type, typename vec_type>
static void check_all_xss_algos(const vec_type &instance) {
  // verify initial instance
  auto res0 = xss_isa_psv::run(instance.data(), instance.size());

  // check_type::check(instance, res0);

  check_all_xss_algos<NAIVE, check_type> (instance, res0);
  check_all_xss_algos<STATIC, check_type> (instance, res0);
  check_all_xss_algos<DYNAMIC, check_type> (instance, res0);
  check_all_xss_algos<DYNAMIC_BUFFERED, check_type> (instance, res0);
}