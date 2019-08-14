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

#include <iostream>
#include <tlx/cmdline_parser.hpp>

#ifdef MALLOC_COUNT
#include <malloc_count.h>
#endif // MALLOC_COUNT

#include <data_structures/ctz/ctz.hpp>
#include <executable/bench.hpp>
#include <util/file_util.hpp>
#include <util/time_measure.hpp>

struct {
  std::vector<std::string> file_paths;
  uint64_t bytes_per_char = 1;
  uint64_t number_of_runs = 5;
  uint64_t prefix_size = 0;
  uint64_t ctz_strategy = 0;
  uint64_t delta = std::numeric_limits<uint64_t>::max();
  uint64_t quantiles = 0;

  bool default_bench = false;
  bool ctz_bench = false;
  bool stack_bench = false;
  bool z_term = false;

  std::vector<uint64_t> deltas;

  std::string contains = "";
  std::string not_contains = "";

  bool matches(const std::string algo) const {
    std::stringstream c(contains);
    while (c.good()) {
      std::string c_sub;
      getline(c, c_sub, ',');
      if (algo.find(c_sub) == std::string::npos)
        return false;
    }

    if (not_contains.size() == 0)
      return true;

    std::stringstream nc(not_contains);
    while (nc.good()) {
      std::string nc_sub;
      getline(nc, nc_sub, ',');
      if (algo.find(nc_sub) != std::string::npos)
        return false;
    }
    return true;
  }

} global_settings;

template <typename ctz_type, typename char_t>
int32_t run(const std::vector<char_t>& vector, const std::string name) {
  const auto& s = global_settings;
  const uint64_t runs = s.number_of_runs;
  std::string additional_info = "file=" + name;

  if (s.stack_bench) {

    std::cout << "Benchmarking all stack implementations." << std::endl;

    if (s.contains.size() + s.not_contains.size() > 0) {
      std::cout << "The filters (--contains / -- not_contains) will be ignored."
                << std::endl;
    }

    run_xss_real<NAIVE, ctz_type>(vector, 0, runs, additional_info);
    for (uint64_t delta = 1; delta <= 128; delta <<= 1) {
      run_xss_real<STATIC, ctz_type>(vector, delta >> 1, runs, additional_info);
    }
    for (uint64_t delta = 1; delta <= 128; delta <<= 1) {
      run_xss_real<DYNAMIC, ctz_type>(vector, delta >> 1, runs,
                                      additional_info);
    }
    for (uint64_t delta = 1; delta <= 128; delta <<= 1) {
      run_xss_real<DYNAMIC_BUFFERED, ctz_type>(vector, delta >> 1, runs,
                                               additional_info);
    }

  } else if (s.default_bench) {

    // linear time stuff goes first
    if (s.matches("xss-real")) {
      for (const auto delta : s.deltas) {
        run_xss_real<DYNAMIC_BUFFERED, ctz_type>(vector, delta, runs,
                                                 additional_info);
        run_xss_real<DYNAMIC, ctz_type>(vector, delta, runs, additional_info);
      }
    }

    if (s.matches("nss-real") || s.matches("nss-real-array")) {
      run_nss_real(vector, runs, additional_info);
    }

    if (s.matches("pss-real") || s.matches("pss-real-array")) {
      run_pss_real(vector, runs, additional_info);
    }

    if (s.matches("lyndon-real") || s.matches("lyndon-real-array")) {
      run_lyndon_real(vector, runs, additional_info);
    }

    if (s.matches("xss-bps-lcp")) {
      for (const auto delta : s.deltas) {
        run_xss_bps_lcp<DYNAMIC_BUFFERED, ctz_type>(vector, delta, runs,
                                                    additional_info);
        run_xss_bps_lcp<DYNAMIC, ctz_type>(vector, delta, runs,
                                           additional_info);
      }
    }

    if (s.matches("xss-bps")) {
      run_xss_bps<DYNAMIC_BUFFERED, ctz_type>(vector, runs, additional_info);
      run_xss_bps<DYNAMIC, ctz_type>(vector, runs, additional_info);
    }

    if (s.matches("bps-support-sada"))
      run_bps_support_sdsl(vector, runs, additional_info);

    if (s.matches("sdsl-lyn-naive"))
      run_sdsl_naive(vector, runs, additional_info);
    if (s.matches("sdsl-lyn-herlez"))
      run_sdsl_herlez(vector, runs, additional_info);
    if (s.matches("sdsl-lyn-herlez-1k"))
      run_sdsl_herlez1k(vector, runs, additional_info);

    if (s.matches("sdsl-lyn-prezza"))
      run_sdsl_prezza(vector, runs, additional_info);
    if (s.matches("sdsl-lyn-prezza-1k"))
      run_sdsl_prezza1k(vector, runs, additional_info);

    if (s.matches("sdsl-isa-nsv"))
      run_sdsl_isa_nsv(vector, runs, additional_info);

    if (s.matches("divsufsort"))
      run_divsufsort(vector, runs, additional_info);

    if (s.matches("gsaca"))
      run_gsaca(vector, runs, additional_info);
    if (s.matches("gsaca-phase1"))
      run_gsaca_phase1(vector, runs, additional_info);
    if (s.matches("gsaca-lyndon"))
      run_gsaca_lyndon(vector, runs, additional_info);

    //    if (s.matches("xss-isa-psv"))
    //      run_xss_isa_psv(vector, runs, additional_info);

    //    if (s.matches("psv-simple"))
    //      run_psv_simple(vector, runs, additional_info);
    //    if (s.matches("psv-sdsl-naive"))
    //      run_psv_sdsl_naive(vector, runs, additional_info);
    //
    //    if (s.matches("pss-sdsl-naive"))
    //      run_pss_sdsl_naive(vector, runs, additional_info);
    //    if (s.matches("pss-sdsl-rk"))
    //      run_pss_sdsl_rk(vector, runs, additional_info);
    //    if (s.matches("pss-sdsl-rk-1k"))
    //      run_pss_sdsl_rk1k(vector, runs, additional_info);
    //
    //    if (s.matches("xss-rk-lce"))
    //      run_xss_simple_rk(vector, runs, additional_info);
    //    if (s.matches("xss-rk-lce-1k"))
    //      run_xss_simple_rk1k(vector, runs, additional_info);
  }

  return 0;
}

template <typename char_t>
int32_t start() {
  if (global_settings.stack_bench && global_settings.file_paths.size() == 0) {
    //    if (global_settings.matches("lcp")) {
    for (uint64_t i = 2; i <= 1024; i <<= 1) {
      bench_stacks(global_settings.prefix_size, i,
                   global_settings.number_of_runs);
    }
    //    }

    //    if (global_settings.matches("indices") ||
    //    global_settings.matches("index")) {
    //        bench_stacks_only_unary(global_settings.prefix_size,
    //                     global_settings.number_of_runs);
    //    }
  }
  for (auto file : global_settings.file_paths) {
    std::vector<char_t> text_vec =
        file_to_instance<char_t>(file, global_settings.prefix_size);
    if (global_settings.z_term && text_vec.size() > 2) {
      text_vec[text_vec.size() - 2] = std::numeric_limits<uint8_t>::max();
    }
    std::cout << "Input ready." << std::endl;

    const auto last_slash_idx = file.find_last_of("/");
    if (last_slash_idx != std::string::npos) {
      file.erase(0, last_slash_idx + 1);
    }

    if (global_settings.quantiles > 0) {
      run_rk1k_distribution(text_vec, "file=" + file,
                            global_settings.quantiles);
    }

    run<ctz_builtin>(text_vec, file);
  }
  return 0;
}

int32_t main(int32_t argc, char const* argv[]) {
  tlx::CmdlineParser cp;

  bool list = false;

  cp.set_description("Nearest Smaller Suffix Construction");
  cp.set_author("Jonas Ellert <jonas.ellert@tu-dortmund.de>");

  cp.add_stringlist('f', "file", global_settings.file_paths,
                    "Path(s) to the text file(s).");

  cp.add_bytes('\0', "runs", global_settings.number_of_runs,
               "Number of repetitions of the algorithm (default = 5).");
  cp.add_bytes('\0', "length", global_settings.prefix_size,
               "Length of the prefix of the text that should be considered.");
  cp.add_bytes('\0', "delta", global_settings.delta,
               "Parameter delta of the LCP stack. (default = 0 and 4)");

  cp.add_flag('\0', "bench-default", global_settings.default_bench,
              "Execute the default benchmark.");
  cp.add_flag('\0', "bench-ctz", global_settings.ctz_bench,
              "Execute the benchmark for trailing / leading zeros.");
  cp.add_flag('\0', "bench-stacks", global_settings.stack_bench,
              "Execute the benchmark for stack implementations.");

  cp.add_bytes('\0', "lce-stats", global_settings.quantiles,
               "Computes LCE statistics with given number of quantiles.");

  cp.add_flag('z', "z", global_settings.z_term,
              "Replace the last character by a maximal character.");

  cp.add_string('\0', "contains", global_settings.contains,
                "Only execute algorithms, that contains at least one of the "
                "given strings (comma separated).");
  cp.add_string('\0', "not-contains", global_settings.not_contains,
                "Only execute algorithms, that contain none of the given "
                "strings (comma separated).");

  cp.add_flag('\0', "list", list, "List the available algorithms.");

  if (!cp.process(argc, argv)) {
    return -1;
  }

  if (list) {
    std::cout << "Algorithms:" << std::endl;
    std::cout << "    "
              << "xss-real" << std::endl;
    std::cout << "    "
              << "xss-bps-lcp" << std::endl;
    std::cout << "    "
              << "xss-bps" << std::endl;
    std::cout << "    "
              << "bps-support-sada" << std::endl;
    std::cout << "    "
              << "sdsl-lyn-naive" << std::endl;
    std::cout << "    "
              << "sdsl-lyn-herlez" << std::endl;
    std::cout << "    "
              << "sdsl-lyn-herlez-1k" << std::endl;
    std::cout << "    "
              << "sdsl-lyn-prezza" << std::endl;
    std::cout << "    "
              << "sdsl-lyn-prezza-1k" << std::endl;
    std::cout << "    "
              << "sdsl-lyn-isa-nsv" << std::endl;
    std::cout << "    "
              << "divsufsort" << std::endl;
    std::cout << "    "
              << "gsaca" << std::endl;
    std::cout << "    "
              << "gsaca-phase1" << std::endl;
    std::cout << "    "
              << "gsaca-lyndon" << std::endl;

    return 0;
  }

  if (global_settings.delta == std::numeric_limits<uint64_t>::max()) {
    global_settings.deltas.push_back(0);
    global_settings.deltas.push_back(4);
  } else {
    global_settings.deltas.push_back(global_settings.delta);
  }

  if ((global_settings.default_bench && global_settings.ctz_bench) ||
      (global_settings.ctz_bench && global_settings.stack_bench) ||
      (global_settings.stack_bench && global_settings.default_bench)) {
    std::cerr << "Please select only one of the following benchmarks: "
              << "--bench-default (default), --bench-ctz, --bench-stacks"
              << std::endl;
    return -1;
  }

  if (!global_settings.ctz_bench && !global_settings.stack_bench &&
      global_settings.quantiles == 0) {
    global_settings.default_bench = true;
  }

  if (global_settings.ctz_bench) {
    bench_ctz(global_settings.prefix_size, global_settings.number_of_runs);
    return 0;
  }

  return start<uint_bytes_t<1>>();
}