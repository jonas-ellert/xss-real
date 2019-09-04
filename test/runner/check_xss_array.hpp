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
#include <nss-real.hpp>
#include <algorithms/nss_isa.hpp>
#include <algorithms/pss_isa.hpp>

template <typename check_type, typename vec_type>
static void check_xss_array(const vec_type &instance) {
  const uint64_t n = instance.size();
  const auto text = instance.data();

  const auto nss_from_isa = nss_isa::run(text, n);
  const auto pss_from_isa = pss_isa::run(text, n);

  uint64_t thresholds [] = { 8, 16, 32, 64, 128, 256, 512 };
  bool modes [] = { nss_real::USE_LCE_AMORTIZE, nss_real::USE_LCE_STACK };

  using index_type = uint32_t;

  for (auto threshold : thresholds) {
    for (auto mode : modes) {
      const auto nss_vec = nss_real::nss(text, n, threshold, mode);
      for (uint64_t i = 0; i < n; ++i) {
        if (nss_vec[i] != (index_type)(nss_from_isa[i])) {
          std::cout << "nss[" << i << "] is " << nss_vec[i] << ", but should be " << nss_from_isa[i] << "." << std::endl;
        }
      }
      const auto pss_vec = nss_real::pss(text, n, threshold, mode);
      for (uint64_t i = 0; i < n; ++i) {
        if (pss_vec[i] != (index_type)(pss_from_isa[i])) {
          std::cout << "pss[" << i << "] is " << nss_vec[i] << ", but should be " << pss_from_isa[i] << "." << std::endl;
        }
      }

      const auto nss_and_pss_vecs = nss_real::xss(text, n, threshold, mode);
      const auto &nss_vec2 = nss_and_pss_vecs.first;
      const auto &pss_vec2 = nss_and_pss_vecs.second;
      for (uint64_t i = 0; i < n; ++i) {
        if (nss_vec2[i] != (index_type)(nss_from_isa[i])) {
          std::cout << "nss[" << i << "] is " << nss_vec2[i] << ", but should be " << nss_from_isa[i] << "." << std::endl;
        }
      }
      for (uint64_t i = 0; i < n; ++i) {
        if (pss_vec2[i] != (index_type)(pss_from_isa[i])) {
          std::cout << "pss[" << i << "] is " << nss_vec2[i] << ", but should be " << pss_from_isa[i] << "." << std::endl;
        }
      }
    }
  }
//

}