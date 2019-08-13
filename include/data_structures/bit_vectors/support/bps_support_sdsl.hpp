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

#include <sdsl/bp_support_sada.hpp>
#include <util/bit_reversal.hpp>
#include <util/common.hpp>

template <typename bp_type>
class bps_support_sdsl {
private:
  const bp_type& bp_;

  const sdsl::bit_vector sdsl_bp_;
  const sdsl::bp_support_sada<> sada_;

  static sdsl::bit_vector to_sdsl(const bp_type& bp) {
    sdsl::bit_vector result(bp.size());
    const uint64_t words = (bp.size() + 63) >> 6;
    const uint64_t* raw_data = bp.data();
    uint64_t* sdsl_data = result.data();
    for (uint64_t i = 0; i < words; ++i) {
      sdsl_data[i] = bit_reversal(raw_data[i]);
    }
    return result;
  }

public:
  bps_support_sdsl(const bp_type& bp)
      : bp_(bp), sdsl_bp_(to_sdsl(bp)), sada_(&sdsl_bp_) {}

  xssr_always_inline uint64_t parent_distance(const uint64_t bps_idx) const {
    const uint64_t bps_idx_open_parent = sada_.enclose(bps_idx);
    return (bps_idx - bps_idx_open_parent + 1) >> 1;
  }

  xssr_always_inline uint64_t subtree_size(const uint64_t bps_idx) const {
    const uint64_t bps_idx_close_nss = sada_.find_close(bps_idx);
    return (bps_idx_close_nss - bps_idx + 1) >> 1;
  }

  xssr_always_inline uint64_t previous_value(const uint64_t preorder_number) const {
    const uint64_t bps_idx_open_node = sada_.select(preorder_number + 2);
    const uint64_t parent_dist = parent_distance(bps_idx_open_node);
    return preorder_number - parent_dist;
  }

  xssr_always_inline uint64_t next_value(const uint64_t preorder_number) const {
    const uint64_t bps_idx_open_node = sada_.select(preorder_number + 2);
    const uint64_t subtree = subtree_size(bps_idx_open_node);
    return preorder_number + subtree;
  }
};