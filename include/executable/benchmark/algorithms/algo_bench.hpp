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

#include <algorithms/psv_simple.hpp>
#include <algorithms/xss_bps.hpp>
#include <algorithms/xss_bps_lcp.hpp>
#include <algorithms/xss_herlez.hpp>
#include <algorithms/xss_isa_psv.hpp>
#include <algorithms/xss_real.hpp>
#include <data_structures/bit_vectors/support/bps_support_sdsl.hpp>
#include <data_structures/lce/lce_prezza.hpp>
#include <data_structures/lce/lce_prezza1k.hpp>
#include <data_structures/lce/lce_stats.hpp>
#include <divsufsort.h>
#include <gsaca.h>
#include <nss-real.hpp>
#include <sdsl/algorithms.hpp>
#include <util/enums.hpp>

enum output_types { array64, array32, bps };

template <output_types type, typename runner_type, typename teardown_type>
void run_generic(const std::string name,
                 const std::string additional_info,
                 runner_type& runner,
                 teardown_type& teardown,
                 const uint64_t n,
                 const uint64_t runs,
                 const uint64_t bpn_offset = 0) {

  static_assert(type == output_types::array32 ||
                type == output_types::array64 || type == output_types::bps);

  std::cout << "RESULT algo=" << name << " ";
  if (additional_info.size() > 0) {
    std::cout << additional_info << " ";
  }
  std::cout << "runs=" << runs << " n=" << n << " " << std::flush;

  std::pair<uint64_t, uint64_t> time_mem = get_time_mem(runner, teardown, runs);

  const uint64_t result_bytes =
      (type != output_types::bps)
          ? ((type != output_types::array32) ? (8 * n) : (4 * n))
          : (n / 4);
  const uint64_t total_memory = time_mem.second + n - (bpn_offset * n / 8);
  const int64_t additional_memory =
      time_mem.second - result_bytes - (bpn_offset * n / 8);

  std::cout << "median_time=" << time_mem.first
            << " total_memory=" << total_memory
            << " additional_memory=" << additional_memory << " ";

  auto mibs = (n / 1024.0 / 1024.0) / (time_mem.first / 1000.0);
  auto bpn = (8.0 * total_memory) / n;
  auto additional_bpn = (8.0 * additional_memory) / n;

  std::cout << "mibs=" << mibs << " bpn=" << bpn
            << " additional_bpn=" << additional_bpn
            << " output_bytes=" << result_bytes << std::endl;
}

template <output_types type, typename runner_type>
void run_generic(const std::string name,
                 const std::string additional_info,
                 runner_type& runner,
                 const uint64_t n,
                 const uint64_t runs,
                 const uint64_t bpn_offset = 0) {
  const auto dummy = []() {};
  run_generic<type>(name, additional_info, runner, dummy, n, runs, bpn_offset);
}

template <typename get_compare_type, typename char_t>
void run_sdsl_generic(const get_compare_type get_compare,
                      const std::vector<char_t>& vector,
                      const uint64_t runs,
                      const std::string additional_info,
                      const std::string sdsl_info,
                      const uint64_t bpn_offset = 0) {
  static_assert(sizeof(unsigned int) == 4);
  const uint64_t n = vector.size() - 1;
  std::vector<char_t> vector_no_front(n);
  vector_no_front.assign(vector.begin() + 1, vector.end());

  const auto func = [&]() {
    auto compare = get_compare(vector_no_front.data(), n);
    std::vector<unsigned int> result(n);
    sdsl::algorithm::calculate_nss(compare, result);
  };

  const auto post = [&]() {
    vector_no_front.assign(vector.begin() + 1, vector.end());
  };

  run_generic<output_types::array32>("sdsl-lyn-" + sdsl_info, additional_info,
                                     func, post, vector.size() - 2, runs,
                                     bpn_offset);
}

template <typename char_t>
void run_sdsl_herlez(const std::vector<char_t>& vector,
                     const uint64_t runs,
                     const std::string additional_info) {
  const auto get_compare = [](char_t* text, const uint64_t n) {
    return lce_herlez<char_t>::get_suffix_compare(text, n);
  };
  run_sdsl_generic(get_compare, vector, runs, additional_info, "herlez");
}

template <typename char_t>
void run_sdsl_herlez1k(const std::vector<char_t>& vector,
                       const uint64_t runs,
                       const std::string additional_info) {
  const auto get_compare = [](char_t* text, const uint64_t n) {
    return lce_herlez1k<char_t>::get_suffix_compare(text, n);
  };
  run_sdsl_generic(get_compare, vector, runs, additional_info, "herlez-1k");
}

template <typename char_t>
void run_sdsl_prezza(const std::vector<char_t>& vector,
                     const uint64_t runs,
                     const std::string additional_info) {
  const auto get_compare = [](char_t* text, const uint64_t n) {
    return lce_prezza<char_t>::get_suffix_compare(text, n);
  };
  run_sdsl_generic(get_compare, vector, runs, additional_info, "prezza", 8);
}

template <typename char_t>
void run_sdsl_prezza1k(const std::vector<char_t>& vector,
                       const uint64_t runs,
                       const std::string additional_info) {
  const auto get_compare = [](char_t* text, const uint64_t n) {
    return lce_prezza1k<char_t>::get_suffix_compare(text, n);
  };
  run_sdsl_generic(get_compare, vector, runs, additional_info, "prezza-1k");
}

template <typename char_t>
void run_sdsl_naive(const std::vector<char_t>& vector,
                    const uint64_t runs,
                    const std::string additional_info) {
  const auto get_compare = [](char_t* text, const uint64_t n) {
    return lce_naive<char_t>::get_suffix_compare(text, n);
  };
  run_sdsl_generic(get_compare, vector, runs, additional_info, "naive");
}

template <typename char_t>
void run_sdsl_isa_nsv(const std::vector<char_t>& vector,
                      const uint64_t runs,
                      const std::string additional_info) {
  static_assert(sizeof(int) == 4);

  const uint64_t n = vector.size();
  const uint64_t sa_bytes = (n - 1) * sizeof(int);

  const auto func = [&]() {
    int* sa = (int*) malloc(sa_bytes);
    divsufsort((unsigned char*) (&(vector.data()[1])), sa, n - 1);

    std::vector<int> isa(n - 1);
    for (uint64_t i = 0; i < n - 1; ++i) {
      isa[sa[i]] = i;
    }
    delete sa;

    std::vector<unsigned int> result(n - 1);
    sdsl::algorithm::calculate_nsv(isa, result);
  };
  run_generic<output_types::array32>("sdsl-lyn-isa-nsv", additional_info, func,
                                     vector.size() - 2, runs);
}

template <typename char_t>
void run_pss_sdsl_rk(const std::vector<char_t>& vector,
                     const uint64_t runs,
                     const std::string additional_info) {
  static_assert(sizeof(unsigned int) == 4);
  const uint64_t n = vector.size() - 1;
  std::vector<char_t> vector_no_front(n);
  vector_no_front.assign(vector.begin() + 1, vector.end());

  const auto func = [&]() {
    auto rk =
        lce_prezza<uint8_t>::get_suffix_compare(vector_no_front.data(), n);
    std::vector<unsigned int> result(n);
    sdsl::algorithm::calculate_pss(rk, result);
  };
  const auto post = [&]() {
    vector_no_front.assign(vector.begin() + 1, vector.end());
  };
  run_generic<output_types::array32>("pss-sdsl-prezza-lce", additional_info,
                                     func, post, vector.size() - 2, runs);
}

template <typename char_t>
void run_pss_sdsl_rk1k(const std::vector<char_t>& vector,
                       const uint64_t runs,
                       const std::string additional_info) {
  static_assert(sizeof(unsigned int) == 4);
  const uint64_t n = vector.size() - 1;
  std::vector<char_t> vector_no_front(n);
  vector_no_front.assign(vector.begin() + 1, vector.end());

  const auto func = [&]() {
    auto rk =
        lce_prezza1k<uint8_t>::get_suffix_compare(vector_no_front.data(), n);
    std::vector<unsigned int> result(n);
    sdsl::algorithm::calculate_pss(rk, result);
  };
  const auto post = [&]() {
    vector_no_front.assign(vector.begin() + 1, vector.end());
  };
  run_generic<output_types::array32>("pss-sdsl-prezza-lce-1k", additional_info,
                                     func, post, vector.size() - 2, runs);
}

template <typename char_t>
void run_psv_sdsl_naive(const std::vector<char_t>& vector,
                        const uint64_t runs,
                        const std::string additional_info) {
  static_assert(sizeof(unsigned int) == 4);
  static_assert(sizeof(unsigned int) == 4);
  const uint64_t n = vector.size() - 1;
  std::vector<char_t> vector_no_front(n);
  vector_no_front.assign(vector.begin() + 1, vector.end());

  const auto func = [&]() {
    std::vector<unsigned int> result(n);
    sdsl::algorithm::calculate_psv(vector_no_front, result);
  };
  run_generic<output_types::array32>("psv-sdsl-naive", additional_info, func,
                                     vector.size() - 2, runs);
}

template <typename char_t>
void run_pss_sdsl_naive(const std::vector<char_t>& vector,
                        const uint64_t runs,
                        const std::string additional_info) {
  static_assert(sizeof(unsigned int) == 4);
  auto naive_compare =
      lce_naive<char_t>::get_suffix_compare(&(vector.data()[1]));
  const auto func = [&]() {
    std::vector<unsigned int> result(vector.size() - 1);
    sdsl::algorithm::calculate_pss(naive_compare, result);
  };
  run_generic<output_types::array32>("pss-sdsl-naive", additional_info, func,
                                     vector.size() - 2, runs);
}

template <typename char_t>
void run_gsaca(const std::vector<char_t>& vector,
               const uint64_t runs,
               const std::string additional_info) {
  static_assert(sizeof(unsigned int) == 4);
  const uint64_t n = vector.size() - 1;
  const uint64_t sa_bytes = n * sizeof(unsigned int);
  unsigned int* sa;
  const auto func = [&]() {
    // skip front sentinel
    sa = static_cast<unsigned int*>(malloc(sa_bytes));
    gsaca_unsigned((unsigned char*) (&(vector.data()[1])), sa, n);
  };
  const auto post = [&]() { delete sa; };
  run_generic<output_types::array32>("gsaca", additional_info, func, post,
                                     vector.size() - 2, runs);
}

template <typename char_t>
void run_gsaca_phase1(const std::vector<char_t>& vector,
                      const uint64_t runs,
                      const std::string additional_info) {
  static_assert(sizeof(unsigned int) == 4);
  const uint64_t n = vector.size() - 1;
  const uint64_t sa_bytes = n * sizeof(unsigned int);
  unsigned int* sa;
  const auto func = [&]() {
    // skip front sentinel
    sa = static_cast<unsigned int*>(malloc(sa_bytes));
    gsaca_unsigned_phase_one((unsigned char*) (&(vector.data()[1])), sa, n);
  };
  const auto post = [&]() { delete sa; };
  run_generic<output_types::array32>("gsaca-phase1", additional_info, func,
                                     post, vector.size() - 2, runs);
}

template <typename char_t>
void run_gsaca_lyndon(const std::vector<char_t>& vector,
                      const uint64_t runs,
                      const std::string additional_info) {
  static_assert(sizeof(unsigned int) == 4);
  const uint64_t n = vector.size() - 1;
  const uint64_t sa_bytes = n * sizeof(unsigned int);
  unsigned int* sa;
  unsigned int* la;
  const auto func = [&]() {
    // skip front sentinel
    sa = static_cast<unsigned int*>(malloc(sa_bytes));
    la = static_cast<unsigned int*>(malloc(sa_bytes));
    gsaca_unsigned_lyndon((unsigned char*) (&(vector.data()[1])), sa, la, n);
  };
  const auto post = [&]() {
    delete sa;
    delete la;
  };
  run_generic<output_types::array32>("gsaca-lyndon", additional_info, func,
                                     post, vector.size() - 2, runs);
}

template <typename char_t>
void run_divsufsort(const std::vector<char_t>& vector,
                    const uint64_t runs,
                    const std::string additional_info) {
  static_assert(sizeof(int) == 4);
  const uint64_t n = vector.size() - 1;
  const uint64_t sa_bytes = n * sizeof(int);
  int* sa;
  const auto func = [&]() {
    // skip front sentinel
    sa = static_cast<int*>(malloc(sa_bytes));
    divsufsort((unsigned char*) (&(vector.data()[1])), sa, n);
  };
  const auto post = [&]() { delete sa; };
  run_generic<output_types::array32>("divsufsort", additional_info, func, post,
                                     vector.size() - 2, runs);
}

template <stack_strategy alloc, typename ctz_type, typename char_t>
void run_xss_real(const std::vector<char_t>& vector,
                  const uint64_t delta,
                  const uint64_t runs,
                  const std::string additional_info) {
  const auto func = [&]() {
    xss_real<alloc, ctz_type>::run(vector.data(), vector.size(), delta);
  };
  const std::string info =
      "ctz_strategy=" + ctz_type::to_string() +
      " stack_type=" + std::to_string(alloc) +
      ((alloc != NAIVE) ? (" delta=" + std::to_string(delta)) : "") +
      ((additional_info.size() > 0) ? " " : "") + additional_info;
  run_generic<output_types::bps>("xss-real", info, func, vector.size() - 2,
                                 runs);
}

template <typename char_t>
void run_nss_real(const std::vector<char_t>& vector,
                  const uint64_t runs,
                  const std::string additional_info) {
  const auto func = [&]() { nss_real::nss(vector.data(), vector.size()); };

  run_generic<output_types::array32>("nss-real", additional_info, func,
                                     vector.size() - 2, runs);
}

template <stack_strategy alloc, typename ctz_type, typename char_t>
void run_xss_bps_lcp(const std::vector<char_t>& vector,
                     const uint64_t delta,
                     const uint64_t runs,
                     const std::string additional_info) {
  const auto func = [&]() {
    xss_bps_lcp<alloc, ctz_type>::run(vector.data(), vector.size(), delta);
  };
  const std::string info =
      "ctz_strategy=" + ctz_type::to_string() +
      " stack_type=" + std::to_string(alloc) +
      ((alloc != NAIVE) ? (" delta=" + std::to_string(delta)) : "") +
      ((additional_info.size() > 0) ? " " : "") + additional_info;
  run_generic<output_types::bps>("xss-bps-lcp", info, func, vector.size() - 2,
                                 runs);
}

template <stack_strategy alloc, typename ctz_type, typename char_t>
void run_xss_bps(const std::vector<char_t>& vector,
                 const uint64_t runs,
                 const std::string additional_info) {
  const auto func = [&]() {
    xss_bps<alloc, ctz_type>::run(vector.data(), vector.size());
  };
  const std::string info = "ctz_strategy=" + ctz_type::to_string() +
                           " stack_type=" + std::to_string(alloc) +
                           ((additional_info.size() > 0) ? " " : "") +
                           additional_info;
  run_generic<output_types::bps>("xss-bps", info, func, vector.size() - 2,
                                 runs);
}

template <typename char_t>
void run_xss_simple_rk(const std::vector<char_t>& vector,
                       const uint64_t runs,
                       const std::string additional_info) {
  const auto func = [&]() {
    auto rk =
        lce_prezza<uint8_t>::get_suffix_compare(vector.data(), vector.size());
    psv_simple<>::run_from_comparison(rk, vector.size());
  };
  run_generic<output_types::bps>("xss-simple-rk-lce", additional_info, func,
                                 vector.size() - 2, runs);
}

template <typename char_t>
void run_xss_simple_rk1k(const std::vector<char_t>& vector,
                         const uint64_t runs,
                         const std::string additional_info) {
  const auto func = [&]() {
    auto rk =
        lce_prezza1k<uint8_t>::get_suffix_compare(vector.data(), vector.size());
    psv_simple<>::run_from_comparison(rk, vector.size());
  };
  run_generic<output_types::bps>("xss-simple-rk-lce-1k", additional_info, func,
                                 vector.size() - 2, runs);
}

template <typename char_t>
void run_rk1k_distribution(const std::vector<char_t>& vector,
                           const std::string additional_info,
                           const uint64_t quantile_cnt) {

  std::cout << "RESULT algo=lce_distribution " << additional_info << " "
            << std::flush;

  auto rk = lce_stats<>::get_suffix_compare(vector.data(), vector.size());
  psv_simple<>::run_from_comparison(rk, vector.size());

  auto& distr = rk.lce_distribution_;

  uint64_t cnt, sum, min, max;
  double avg;
  std::vector<std::pair<double, double>> quantiles(quantile_cnt);
  std::vector<std::pair<double, double>> additional_quantiles;
  distr.get_stats(cnt, sum, min, max, avg, quantiles, additional_quantiles);

  std::cout << "n=" << vector.size() << " "
            << "lce_count=" << cnt << " "
            << "longest_chain=" << distr.longest_chain << " "
            << "longest_chain_percent="
            << distr.longest_chain / ((double) vector.size()) << " "
            << "lce_sum=" << sum << " "
            << "lce_min=" << min << " "
            << "lce_max=" << max << " "
            << "lce_avg=" << avg << " "
            << "quantiles=(0, " << min << ")";

  for (uint64_t i = 0; i < quantile_cnt; ++i) {
    std::cout << "(" << quantiles[i].first << ", " << quantiles[i].second
              << ")";
  }
  for (uint64_t i = 0; i < additional_quantiles.size(); ++i) {
    std::cout << "(" << additional_quantiles[i].first << ", "
              << additional_quantiles[i].second << ")";
  }
  std::cout << "(1, " << max << ") " << std::flush;

  xss_real<NAIVE>::run<true>(vector.data(), vector.size(), 0);
  std::cout << "skipped_by_re=" << xss_real_stats.skipped_re << " ";
  std::cout << "skipped_by_al=" << xss_real_stats.skipped_al << " ";
  std::cout << "skipped_by_re_percent="
            << xss_real_stats.skipped_re / ((double) vector.size()) << " ";
  std::cout << "skipped_by_al_percent="
            << xss_real_stats.skipped_al / ((double) vector.size())
            << std::endl;
}

template <typename char_t>
void run_xss_isa_psv(const std::vector<char_t>& vector,
                     const uint64_t runs,
                     const std::string additional_info) {
  const auto func = [&]() { xss_isa_psv::run(vector.data(), vector.size()); };
  run_generic<output_types::bps>("xss-isa-psv", additional_info, func,
                                 vector.size() - 2, runs);
}

template <typename char_t>
void run_psv_simple(const std::vector<char_t>& vector,
                    const uint64_t runs,
                    const std::string additional_info) {
  const auto func = [&]() { psv_simple<>::run(vector.data(), vector.size()); };
  run_generic<output_types::bps>("psv-simple", additional_info, func,
                                 vector.size() - 2, runs);
}

template <typename char_t>
void run_bps_support_sdsl(const std::vector<char_t>& vector,
                          const uint64_t runs,
                          const std::string additional_info) {
  auto bps = xss_real<>::run(vector.data(), vector.size());
  const auto func = [&]() { volatile bps_support_sdsl build(bps); };

  run_generic<output_types::bps>("bps-support-sada", additional_info, func,
                                 vector.size() - 2, runs);
}