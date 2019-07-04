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

#include <data_structures/lce/rk/rk_lce.hpp>
#include <map>
#include <util/common.hpp>

template <typename value_type = uint8_t>
class lce_stats {
public:
  struct distribution {
    std::map<uint64_t, uint64_t> map;
    int64_t longest_chain = 0;
    int64_t current_chain = 0;

    always_inline void add(const uint64_t lce, bool smaller) {
      ++map[lce];
      current_chain += smaller ? 1 : -1;
      longest_chain = std::max(longest_chain, current_chain);
    }

    always_inline void get_stats(uint64_t& cnt,
                                 uint64_t& sum,
                                 uint64_t& min,
                                 uint64_t& max,
                                 double& average) {
      get_base_stats(cnt, sum, min, max);
      average = ((double) sum) / cnt;
    }

    always_inline void
    get_stats(uint64_t& cnt,
              uint64_t& sum,
              uint64_t& min,
              uint64_t& max,
              double& avg,
              std::vector<std::pair<double, double>>& quantiles,
              std::vector<std::pair<double, double>>& additional_quantiles) {
      get_stats(cnt, sum, min, max, avg);
      get_quantiles(cnt, quantiles, additional_quantiles);
    }

  private:
    always_inline void
    get_base_stats(uint64_t& cnt, uint64_t& sum, uint64_t& min, uint64_t& max) {
      cnt = sum = 0;
      for (const auto& pair : map) {
        cnt += pair.second;
        sum += pair.first * pair.second;
      }

      auto begin = map.cbegin();
      auto end = map.cend();
      --end;
      min = (*begin).first;
      max = (*end).first;
    }

    always_inline void get_quantiles(
        const uint64_t cnt,
        std::vector<std::pair<double, double>>& quantiles,
        std::vector<std::pair<double, double>>& additional_quantiles) {
      double interval_width = 1.0 / (quantiles.size() + 1);

      std::vector<uint64_t> indices;
      indices.reserve(quantiles.size() << 1);
      double target = interval_width;
      for (uint64_t i = 0; i < quantiles.size(); ++i) {
        indices.push_back(std::floor(target * (cnt - 1)));
        indices.push_back(std::ceil(target * (cnt - 1)));
        quantiles[i].first = target;
        target += interval_width;
      }
      target -= interval_width;

      constexpr double additional_targets[46] = {
          0.9,      0.91,     0.92,     0.93,     0.94,     0.95,     0.96,
          0.97,     0.98,     0.99,     0.991,    0.992,    0.993,    0.994,
          0.995,    0.996,    0.997,    0.998,    0.999,    0.9991,   0.9992,
          0.9993,   0.9994,   0.9995,   0.9996,   0.9997,   0.9998,   0.9999,
          0.99991,  0.99992,  0.99993,  0.99994,  0.99995,  0.99996,  0.99997,
          0.99998,  0.99999,  0.999991, 0.999992, 0.999993, 0.999994, 0.999995,
          0.999996, 0.999997, 0.999998, 0.999999};

      uint64_t a = 0;
      while (a < 46 && additional_targets[a] <= target) {
        ++a;
      }

      if (a < 64 &&
          std::to_string(additional_targets[a]) == std::to_string(target)) {
        ++a;
      }

      additional_quantiles.clear();
      std::vector<uint64_t> additional_indices;
      for (; a < 46; ++a) {
        additional_indices.push_back(
            std::floor(additional_targets[a] * (cnt - 1)));
        additional_indices.push_back(
            std::ceil(additional_targets[a] * (cnt - 1)));
        additional_quantiles.emplace_back(additional_targets[a], 0);
      }

      auto it = map.cbegin();
      uint64_t current_idx = (*it).second;
      for (uint64_t i = 0; i < indices.size(); ++i) {
        while (current_idx < indices[i]) {
          ++it;
          current_idx += (*it).second;
        }
        quantiles[i >> 1].second += 0.5 * (*it).first;
      }

      for (uint64_t i = 0; i < additional_indices.size(); ++i) {
        while (current_idx < additional_indices[i]) {
          ++it;
          current_idx += (*it).second;
        }
        additional_quantiles[i >> 1].second += 0.5 * (*it).first;
      }
    }
  };

  struct lce {
    const value_type* text_;
    rklce::rk_lce rk_lce_;
    distribution lce_distribution_;
    lce(const value_type* text, const uint64_t n)
        : text_(text), rk_lce_(text, n) {}
    always_inline uint64_t operator()(const uint64_t i, const uint64_t j) {
      uint64_t l = 0;
      while (text_[i + l] == text_[j + l] && likely(l < 1000))
        ++l;
      l = likely(text_[i + l] != text_[j + l])
              ? l
              : (l + rk_lce_.LCE(i + l, j + l));
      lce_distribution_.add(l, text_[std::min(i, j) + l] <
                                   text_[std::max(i, j) + l]);
      return l;
    }
  };

  struct suffix_compare {
    const value_type* text_;
    lce rk_lce_;
    distribution& lce_distribution_;
    suffix_compare(const value_type* text, const uint64_t n)
        : text_(text),
          rk_lce_(text, n),
          lce_distribution_(rk_lce_.lce_distribution_) {}
    always_inline uint64_t operator()(const uint64_t i, const uint64_t j) {
      uint64_t lce = rk_lce_(i, j);
      return text_[i + lce] < text_[j + lce];
    }
  };

  always_inline static lce get_lce(const value_type* text, const uint64_t n) {
    return lce(text, n);
  }

  always_inline static suffix_compare get_suffix_compare(const value_type* text,
                                                         const uint64_t n) {
    return suffix_compare(text, n);
  }

private:
  lce_stats() {}
};