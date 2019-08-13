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
#include <algorithms/nss_real.hpp>
#include <algorithms/nss_isa.hpp>

template <typename check_type, typename vec_type>
static void check_xss_array(const vec_type &instance) {
  const uint64_t n = instance.size();
  const auto text = instance.data();
  std::vector<uint32_t> nss_vec(n);
  nss_real::run(text, nss_vec.data(), n);




  const auto from_isa = nss_isa::run(text, n);
  for (uint64_t i = 0; i < n; ++i) {
    if (nss_vec[i] != (uint32_t)(from_isa[i])) {
      std::cout << "nss[" << i << "] is " << nss_vec[i] << ", but should be " << from_isa[i] << "." << std::endl;
    }

  }

//  check_type::check_nss(instance, nss_vec);
}