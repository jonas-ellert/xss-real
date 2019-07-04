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

#include <gtest/gtest.h>

#include "util/test_check.hpp"
#include "util/test_lookahead.hpp"
#include <algorithms/xss_real.hpp>

using check_type = nss_check<true, true>;

template <typename instance_collection>
static void hand_selected_test(instance_collection &&instances) {
  std::cout << "Number of instances: " << instances.size() << std::endl;
  uint64_t count = 0;
  for (auto instance : instances) {
    std::cout << "Testing instance " << ++count
              << " (of length " << instance.size() << "): " << std::flush;

    auto res = xss_real<NAIVE, ctz_builtin>::run(instance.data(), instance.size());
    check_type::check(instance, res);
    std::cout << "Forwards done. " << std::flush;

    std::reverse(instance.begin(), instance.end());
    res = xss_real<NAIVE, ctz_builtin>::run(instance.data(), instance.size());
    check_type::check(instance, res);
    std::cout << "Backwards done." << std::endl;
  }
}

TEST(xss, lookahead) {
  std::cout << "Testing XSS with hand selected instances "
            <<"(cover all lookahead cases)." << std::endl;
  hand_selected_test(manual_test_instances_lookahead(1000000));
}