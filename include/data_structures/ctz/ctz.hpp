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

#include <data_structures/ctz/ctz_lookup.hpp>
#include <data_structures/ctz/ctz_plain.hpp>
#include <data_structures/ctz/ctz_strategy.hpp>

template <ctz_strategy ctz_strategy, uint64_t lookup_bits = 0>
struct ctz_auto {
private:
  constexpr static bool is(const uint64_t strat) {
    return (strat == ctz_strategy);
  }

  template <uint64_t strat, typename t1, typename t2>
  using check_strategy = typename std::conditional<is(strat), t1, t2>::type;

public:
  using type =
      check_strategy<CTZ_BUILTIN,
                     ctz_builtin,
                     check_strategy<CTZ_LINEAR,
                                    ctz_linear<lookup_bits>,
                                    check_strategy<CTZ_LOGARITHMIC,
                                                   ctz_logarithmic<lookup_bits>,
                                                   ctz_debruijn>>>;
};