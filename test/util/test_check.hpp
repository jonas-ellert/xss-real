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

#include <omp.h>

#include <data_structures/bit_vectors/support/bps_support_naive.hpp>
#include <data_structures/bit_vectors/support/bps_support_sdsl.hpp>

template <
    bool abort_on_error,
    bool report_input_text_on_error,
    uint64_t max_text_output_characters = 1ULL * 1024,
    uint64_t max_index_reports_per_check = 5>
struct nss_check {

  constexpr static uint64_t zero_errors = 0;

  inline static bool is_smaller(const std::vector<uint8_t> &text, uint64_t i, uint64_t j) {
    while (text[i] == text[j]) { ++i; ++j; }
    return (text[i] < text[j]);
  }

  inline static bool is_larger(const std::vector<uint8_t> &text, uint64_t i, uint64_t j) {
    while (text[i] == text[j]) { ++i; ++j; }
    return (text[i] > text[j]);
  }

  template <typename vec_type>
  inline static void report_text(const vec_type &text) {
    if constexpr (report_input_text_on_error) {
      if (true || text.size() <= max_text_output_characters) {
        std::cout << "\nText: " << std::endl;
        for (uint64_t i = 0; i < text.size(); ++i)
          std::cout << (uint8_t)((text[i] == '\0') ? '$' : text[i]);
        std::cout << std::endl;
      }
      else {
        std::cout << "\n[text is too long to print]" << std::endl;
      }
    }
  }

  template <typename vec_type>
  inline static void abort_after_error_report(const vec_type &text) {
    if constexpr (abort_on_error) {
      report_text(text);
      std::abort();
    }
  }

  // real xss is closer than calculated xss
  template <typename vec_type>
  inline static void report_missed_xss(const vec_type &text, const uint64_t i, const uint64_t j, const uint64_t xss_i, uint64_t &error_count) {
    #pragma omp critical(report_xss_failure)
    {
      if (!abort_on_error || error_count == 0) {

        if (error_count < max_index_reports_per_check)
          std::cout << "\nWrong result for xss(" << i
                    << "): Reported value is " << xss_i
                    << ", but actual value is " << j << "." << std::flush;
        else if (error_count == max_index_reports_per_check)
          std::cout << "\n[reached maximum number of error reports]" << std::flush;
        ++error_count;
      }
      abort_after_error_report(text);
    }
  }

  // real xss is further than calculated xss
  template <typename vec_type>
  inline static void report_wrong_xss(const vec_type &text, const uint64_t i, const uint64_t xss_i, uint64_t &error_count) {
    #pragma omp critical(report_xss_failure)
    {
      if (!abort_on_error || error_count == 0) {
        if (error_count < max_index_reports_per_check)
          std::cout << "\nWrong result for xss(" << i
                    << "): Reported value is " << xss_i
                    << ", but actual value is further away." << std::flush;
        else if (error_count == max_index_reports_per_check)
          std::cout << "\n[reached maximum number of error reports]" << std::flush;
        ++error_count;
      }
      abort_after_error_report(text);
    }
  }

  template <typename vec_type, typename bp_type>
  inline static void check(const vec_type &text, const bp_type &bps_of_pss_tree) {
    uint64_t error_count = 0;

    const bps_support_naive support(bps_of_pss_tree);
    const bps_support_sdsl support2(bps_of_pss_tree);

    #pragma omp parallel for
    for (uint64_t i = 1; i < text.size() - 1; ++i) {
      const auto nss = support.next_value(i);

      if(support.next_value(i) != support2.next_value(i)) {
        std::cout << support.next_value(i) << std::endl;
        std::cout << support2.next_value(i) << std::endl;
        std::abort();
      }

      for(uint64_t j = i + 1; j < nss; ++j) {
        if (xssr_unlikely(is_smaller(text, j, i))) {
          report_missed_xss(text, i, j, nss, error_count);
        }
      }
      if (xssr_unlikely(is_larger(text, nss, i))) {
        report_wrong_xss(text, i, nss, error_count);
      }
    }

    #pragma omp parallel for
    for (uint64_t i = 1; i < text.size() - 1; ++i) {
      const auto pss = support.previous_value(i);

      if(support.previous_value(i) != support2.previous_value(i)) {
        std::cout << support.previous_value(i) << std::endl;
        std::cout << support2.previous_value(i) << std::endl;
        std::abort();
      }

      for(uint64_t j = i - 1; j > pss; --j) {
        if (xssr_unlikely(is_smaller(text, j, i))) {
          report_missed_xss(text, i, j, pss, error_count);
        }
      }
      if (xssr_unlikely(is_larger(text, pss, i))) {
        report_wrong_xss(text, i, pss, error_count);
      }
    }

    if (error_count > 0) {
      report_text(text);
      std::cout << std::endl;
      EXPECT_EQ(error_count, zero_errors);
      std::cout << std::endl;
    }
  }

};