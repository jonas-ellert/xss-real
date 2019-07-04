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

#include <cmath>
#include <executable/gen.hpp>
#include <iostream>
#include <tlx/cmdline_parser.hpp>
#include <util/common.hpp>
#include <util/file_util.hpp>

struct {
  std::string target = "";
  uint64_t length = 0;
  bool all_prefixes = false;

  uint64_t run_abc = 0;
  uint64_t run_aab = 0;
  uint64_t run_of_runs = 0;
  uint64_t random = 0;
  uint64_t seed = 0;
  bool fibonacci_aab = false;
  bool fibonacci_abc = false;
  bool overlap = false;
  bool pizza_chili = false;
} s;

[[maybe_unused]] static void
print(const std::vector<uint8_t>& vec,
      const uint64_t n = std::numeric_limits<uint64_t>::max()) {
  const uint64_t limit = std::min(vec.size(), n);
  for (uint64_t i = 0; i < limit; ++i) {
    std::cout << (char) vec[i];
  }
  std::cout << std::endl;
}

[[maybe_unused]] static void print_prefix(const std::vector<uint8_t>& vec) {
  std::cout << "Prefix: ";
  print(vec, 72);
}

static bool check_file(const std::string& path) {
  if (std::experimental::filesystem::exists(path)) {
    std::cerr << "File already exists: \"" << path << "\". Skipping."
              << std::endl;
    return false;
  }
  return true;
}

void run_pc() {
  gen_pizza_chili(s.target, s.all_prefixes);
}

int32_t run() {
  if (s.length == 0) {
    std::cerr << "A length must be specified (--length)." << std::endl;
    return -1;
  }

  // RANDOM
  if (s.random) {
    while (s.seed == 0) {
      s.seed = get_random_seed();
    }
    std::cout << "Generating random string." << std::endl;
    std::cout << "Length: " << s.length << ", "
              << "Alphabet: " << s.random << ", "
              << "Seed: " << s.seed << "." << std::endl;
    if (s.random <= gen_alphabet_size) {
      const std::string filename = "random.seed" + std::to_string(s.seed) +
                                   "." + "sigma" + std::to_string(s.random) +
                                   "." + std::to_string(s.length);
      const std::string path = s.target + filename;
      if (check_file(path)) {
        auto result = gen_random(s.length, s.random, s.seed);
        print_prefix(result);
        instance_to_file(path, result, result.size());
      }
    } else {
      std::cerr << "Maximum alphabet size for random strings: "
                << gen_alphabet_size << "." << std::endl
                << "Skipping random string generation." << std::endl;
    }
  }

  if (s.fibonacci_aab) {
    const std::string fib0 = concat(gen_alphabet[0]);
    const std::string fib1 = concat(gen_alphabet[0], gen_alphabet[1]);
    std::cout << "Generating fibonacci string." << std::endl;
    std::cout << "Length: " << s.length << ", "
              << "Fib0: \"" << fib0 << "\", "
              << "Fib1: \"" << fib1 << "\"." << std::endl;

    const std::string filename = "fibonacci.a.ab." + std::to_string(s.length);
    const std::string path = s.target + filename;
    if (check_file(path)) {
      auto result = gen_fibonacci(s.length, fib0, fib1);
      print_prefix(result);
      instance_to_file(path, result, result.size());
    }
  }

  if (s.fibonacci_abc) {
    const std::string fib0 = concat(gen_alphabet[0]);
    const std::string fib1 = concat(gen_alphabet[1], gen_alphabet[2]);
    std::cout << "Generating fibonacci string." << std::endl;
    std::cout << "Length: " << s.length << ", "
              << "Fib0: \"" << fib0 << "\", "
              << "Fib1: \"" << fib1 << "\"." << std::endl;

    const std::string filename = "fibonacci.a.bc." + std::to_string(s.length);
    const std::string path = s.target + filename;
    if (check_file(path)) {
      auto result = gen_fibonacci(s.length, fib0, fib1);
      print_prefix(result);
      instance_to_file(path, result, result.size());
    }
  }

  if (s.overlap) {
    std::cout << "Generating overlapping string." << std::endl;
    std::cout << "Length: " << s.length << std::endl;

    const std::string filename = "overlap." + std::to_string(s.length);
    const std::string path = s.target + filename;
    if (check_file(path)) {
      auto result = gen_overlap(s.length);
      print_prefix(result);
      instance_to_file(path, result, result.size());
    }
  }

  if (s.run_of_runs) {
    std::cout << "Generating run of runs string." << std::endl;
    std::cout << "Length: " << s.length << ", "
              << "Repetitions: " << s.run_of_runs << "." << std::endl;

    if (s.run_of_runs < 2) {
      std::cerr << "Repetitions must be at least 2." << std::endl
                << "Skipping run of runs string generation." << std::endl;
    } else {
      const std::string filename = "run_of_runs.repetitions." +
                                   std::to_string(s.run_of_runs) + "." +
                                   std::to_string(s.length);
      const std::string path = s.target + filename;
      if (check_file(path)) {
        auto result = gen_run_of_runs(s.length, s.run_of_runs);
        print_prefix(result);
        instance_to_file(path, result, result.size());
      }
    }
  }

  if (s.run_abc) {
    const std::string word =
        concat(gen_alphabet[0], gen_alphabet[1], gen_alphabet[2]) + "..";
    std::cout << "Generating run string." << std::endl;
    std::cout << "Length: " << s.length << ", "
              << "Period: " << s.run_abc << ", "
              << "Word: " << word << "." << std::endl;

    if (s.run_abc > gen_alphabet_size) {
      std::cerr << "Maximum period for run-abc strings: " << gen_alphabet_size
                << "." << std::endl
                << "Skipping run-abc string generation." << std::endl;
    } else {
      const std::string filename = "run.abc.period." +
                                   std::to_string(s.run_abc) + "." +
                                   std::to_string(s.length);
      const std::string path = s.target + filename;
      if (check_file(path)) {
        auto result = gen_run_abc(s.length, s.run_abc);
        print_prefix(result);
        instance_to_file(path, result, result.size());
      }
    }
  }

  if (s.run_aab) {
    const std::string word = concat(gen_alphabet[0]) + ".." +
                             concat(gen_alphabet[0], gen_alphabet[1]);
    std::cout << "Generating run string." << std::endl;
    std::cout << "Length: " << s.length << ", "
              << "Period: " << s.run_abc << ", "
              << "Word: " << word << "." << std::endl;

    if (s.run_abc > gen_alphabet_size) {
      std::cerr << "Maximum period for run-abc strings: " << gen_alphabet_size
                << "." << std::endl
                << "Skipping run-abc string generation." << std::endl;
    } else {
      const std::string filename = "run.aab.period." +
                                   std::to_string(s.run_abc) + "." +
                                   std::to_string(s.length);
      const std::string path = s.target + filename;
      if (check_file(path)) {
        auto result = gen_run_aab(s.length, s.run_abc);
        print_prefix(result);
        instance_to_file(path, result, result.size());
      }
    }
  }

  if (s.all_prefixes && s.length > (1024ULL * 1024)) {
    const uint64_t lengthlog = (uint64_t) std::ceil(std::log2(s.length >> 1));
    s.length = 1ULL << lengthlog;
    return run();
  }

  return 0;
}

int32_t main(int32_t argc, char const* argv[]) {
  tlx::CmdlineParser cp;

  cp.set_description("String generator.");
  cp.set_author("Jonas Ellert <jonas.ellert@tu-dortmund.de>");

  cp.add_string('\0', "target", s.target, "Directory for output files.");
  cp.add_bytes('\0', "length", s.length, "Characters per generated string.");
  cp.add_flag('\0', "prefixes", s.all_prefixes, "Build all prefixes.");

  cp.add_bytes(
      '\0', "run-abc", s.run_abc,
      "Build run of given period (e.g. \"abcabcabcabc...\" for period 3).");
  cp.add_bytes(
      '\0', "run-aab", s.run_aab,
      "Build run of given period (e.g. \"aabaabaabaab...\" for period 3).");
  cp.add_bytes(
      '\0', "run-of-runs", s.run_of_runs,
      "Build run of runs of given repetitions (e.g. "
      "\"abcccbcccbcccabcccbcccbcccabcccbcccbccc\" for 3 repetitions).");
  cp.add_bytes('\0', "random", s.random,
               "Build random string of given alphabet size (at most 255). "
               "An optional seed can be provided (--seed).");
  cp.add_bytes('\0', "seed", s.seed, "Seed for random strings.");
  cp.add_flag('\0', "fibonacci-abc", s.fibonacci_abc,
              "Build fibonacci string (with fib0=\"a\" and fib1=\"bc\").");
  cp.add_flag('\0', "overlap", s.overlap, "Build strongly overlapping string.");
  cp.add_flag('\0', "fibonacci-aab", s.fibonacci_aab,
              "Build fibonacci string (with fib0=\"a\" and fib1=\"ab\").");
  cp.add_flag('\0', "pc", s.pizza_chili,
              "Download the pizza chili text collection"
              "(http://pizzachili.dcc.uchile.cl/texts.html).");

  if (!cp.process(argc, argv)) {
    std::cerr << "Could not parse arguments." << std::endl;
    return -1;
  }

  bool run_normal = true;
  if (!(s.run_aab || s.run_abc || s.run_of_runs || s.random ||
        s.fibonacci_aab || s.fibonacci_abc || s.overlap)) {
    run_normal = false;
    if (!s.pizza_chili) {
      std::cerr << "At least one string type must be specified "
                << "(--help for all options)." << std::endl;
      return -1;
    }
  }

  if (s.target.size() == 0) {
    std::cerr << "A target directory must be specified (--target)."
              << std::endl;
    return -1;
  }

  if (mkdir(s.target) != 0) {
    std::cerr << "Could not create target directory \"" << s.target << "\"."
              << std::endl;
    return -1;
  }

  if (s.target[s.target.size() - 1] != '/') {
    s.target = s.target + '/';
  }

  std::cout << "Writing generated files to \"" << s.target << "\"."
            << std::endl;

  if (s.pizza_chili) {
    run_pc();
  }

  if (run_normal) {
    return run();
  }

  return 0;
}