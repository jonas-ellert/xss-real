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

#include <random>
#include <iterator>
#include <fstream>
#include <streambuf>

#include <omp.h>

#ifdef MALLOC_COUNT
#include <malloc_count.h>
#else
#include <MALLOC COUNT NOT ENABLED IN CMAKE>
#endif // MALLOC_COUNT

#include <algorithm/xss_real.hpp>
#include <util/time_measure.hpp>
#include "util/test_files.hpp"


const static std::string size_suffix = ".50MB";
const static std::string archive_suffix = ".gz";

const static std::string file_sources = "sources" + size_suffix;
const static std::string file_proteins = "proteins" + size_suffix;
const static std::string file_dna = "dna" + size_suffix;
const static std::string file_english = "english" + size_suffix;
const static std::string file_xml = "dblp.xml" + size_suffix;

const static std::string archive_sources = file_sources + archive_suffix;
const static std::string archive_proteins = file_proteins + archive_suffix;
const static std::string archive_dna = file_dna + archive_suffix;
const static std::string archive_english = file_english + archive_suffix;
const static std::string archive_xml = file_xml + archive_suffix;

const static std::string dl_sources = "http://pizzachili.dcc.uchile.cl/texts/code/" + archive_sources;
const static std::string dl_proteins = "http://pizzachili.dcc.uchile.cl/texts/protein/" + archive_proteins;
const static std::string dl_dna = "http://pizzachili.dcc.uchile.cl/texts/dna/" + archive_dna;
const static std::string dl_english = "http://pizzachili.dcc.uchile.cl/texts/nlang/" + archive_english;
const static std::string dl_xml = "http://pizzachili.dcc.uchile.cl/texts/xml/" + archive_xml;

const static std::string file_list [5] = { file_sources, file_proteins, file_dna, file_english, file_xml };
const static std::string archive_list [5] = { archive_sources, archive_proteins, archive_dna, archive_english, archive_xml };
const static std::string dl_list [5] = { dl_sources, dl_proteins, dl_dna, dl_english, dl_xml };


static void download(const std::string url, const std::string target_prefix) {
  [[maybe_unused]]
  auto code = system(("wget -nc " + url + ((target_prefix.size() > 0) ? (" -P " + target_prefix) : "")).c_str());
}

static void extract(const std::string file_name, const std::string file_prefix) {
  [[maybe_unused]]
  auto code = system(("yes n | gunzip -kv " + ((file_prefix.size() > 0) ? file_prefix : "") + file_name).c_str());
}


template <stack_strategy strategy, ds_direction_flag direction, typename input_type>
static void test_mem_time(const input_type &input, const uint64_t delta) {
  time_measure time_measurement;

  malloc_count_reset_peak();
  uint64_t mem_pre = malloc_count_current();
  time_measurement.begin();

  auto res = xss_real<strategy>::run(input.data(), input.size(), delta);

  time_measurement.end();
  uint64_t mem_peak = malloc_count_peak();

  const auto input_size = (input.size() / 1024.0 / 1024);
  const auto peak = (mem_peak - mem_pre) / 1024.0 / 1024;
  const auto bpn = (8.0 * (mem_peak - mem_pre)) / (input.size());
  const auto time = time_measurement.millis() / 1000.0;
  const auto speed = input_size / time;

  std::cout << std::to_string(strategy) << "_" << delta << ": "
            << "n = " << input_size << "MB"
            << ", mem = " << peak << "MB (" << bpn << " bpn)"
            << ", time = " << time << "s"
            << ", speed = " << speed << "MB/s"
            << std::endl;
}


TEST(nearest_smaller_suffix_ltr, pizzachili_download) {
  mkdir_file_prefix();

  for (uint64_t i = 0; i < 5; ++i) {
    const auto file = file_list[i];
    const auto archive = archive_list[i];
    const auto dl = dl_list[i];

    download(dl, file_prefix);
    extract(archive, file_prefix);

    std::cout << std::endl;
  }

  for (uint64_t i = 0; i < 5; ++i) {
    std::cout << "Testing " << file_list[i] << ". Direction: "
              << std::to_string(PREVIOUS) << "... " << std::endl;
    auto vec = instance_from_file(file_list[i], 0);
    vec[0] = '\0';
    vec[vec.size() - 1] = '\0';
    test_mem_time<NAIVE, PREVIOUS>(vec, 0);
    test_mem_time<STATIC, PREVIOUS>(vec, 0);
    for (uint64_t delta = 1; delta <= 32; delta = delta << 1)
      test_mem_time<STATIC, PREVIOUS>(vec, delta);
    test_mem_time<DYNAMIC, PREVIOUS>(vec, 0);
    for (uint64_t delta = 1; delta <= 32; delta = delta << 1)
      test_mem_time<DYNAMIC, PREVIOUS>(vec, delta);
    test_mem_time<DYNAMIC_BUFFERED, PREVIOUS>(vec, 0);
    for (uint64_t delta = 1; delta <= 32; delta = delta << 1)
      test_mem_time<DYNAMIC_BUFFERED, PREVIOUS>(vec, delta);
  }
  std::abort();
}
