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

#include <algorithm>
#include <fstream>
#include <vector>

#include <experimental/filesystem>
#include <util/integer_types.hpp>
#include <util/units.hpp>

[[maybe_unused]] static auto mkdir(const std::string path) {
  const static std::string command = "mkdir -p " + path;
  return system(command.c_str());
}

template <typename container_type>
static void vec_to_file(const std::string path,
                        const container_type& instance,
                        const uint64_t n) {
  if (std::experimental::filesystem::exists(path)) {
    std::cerr << "File already exists: \"" << path << "\". Skipping."
              << std::endl;
  } else {
    std::ofstream outFile(path);
    for (uint64_t i = 0; i < n; ++i) {
      outFile << instance[i];
    }
    std::cout << "Saved file: \"" << path << "\"." << std::endl;
  }
}

template <typename container_type>
static void instance_to_file(const std::string path,
                             const container_type& instance,
                             const uint64_t n) {
  if (std::experimental::filesystem::exists(path)) {
    std::cerr << "File already exists: \"" << path << "\". Skipping."
              << std::endl;
  } else {
    std::ofstream outFile(path);
    for (uint64_t i = 0; i < n; ++i) {
      outFile << instance[i];
    }
    std::cout << "Saved file: \"" << path << "\"." << std::endl;
  }
}

template <typename char_t>
static void standardize(std::vector<char_t>& vector) {
  constexpr char_t min_val = std::numeric_limits<char_t>::min();
  constexpr char_t max_val = std::numeric_limits<char_t>::max();

  std::vector<bool> count_sigma(1ULL << (sizeof(char_t) * 8), false);

  char_t min_current = max_val;
  char_t max_current = min_val;
  for (uint64_t i = 1; i < vector.size() - 1; ++i) {
    const auto character = vector[i];
    min_current = std::min(min_current, character);
    max_current = std::max(max_current, character);
    count_sigma[character] = true;
  }

  uint64_t sigma = 0;
  for (const auto b : count_sigma) {
    sigma += b ? 1 : 0;
  }
  std::cout << "[STANDARDIZE]         Alphabet size: sigma=" << sigma << "."
            << std::endl;

  if (min_current == min_val) {
    if (max_current == max_val) {
      std::cerr
          << "[STANDARDIZE ERROR]   Text contains both min_chars and max_chars."
          << std::endl;
      std::cerr
          << "[STANDARDIZE WARNING] Replacing all min_chars with 1-chars.\n"
          << "                      This may influence the resulting data "
             "structures."
          << std::endl;
      for (auto& character : vector)
        if (unlikely(character == min_val))
          ++character;
    } else {
      std::cout << "[STANDARDIZE]         Text contains min_chars, but does "
                   "not contain max_chars."
                << std::endl;
      std::cout << "[STANDARDIZE]         Incrementing all characters by 1."
                << std::endl;
      for (auto& character : vector)
        ++character;
    }
  }

  std::cout
      << "[STANDARDIZE]         Adding sentinels at beginning and end of text."
      << std::endl;
  vector[0] = min_val;
  vector[vector.size() - 1] = min_val;
}

// adds sentinels
template <typename char_t>
static std::vector<char_t> file_to_instance(const std::string& file_name,
                                            const uint64_t prefix_size) {
  std::ifstream stream(file_name.c_str(), std::ios::in | std::ios::binary);

  if (!stream) {
    std::cerr << "File " << file_name << " not found.\n";
    exit(EXIT_FAILURE);
  }

  stream.seekg(0, std::ios::end);
  uint64_t size_in_characters = stream.tellg() / sizeof(char_t);
  stream.seekg(0);

  if (prefix_size > 0) {
    size_in_characters = std::min(prefix_size, size_in_characters);
  }
  uint64_t size_in_bytes = size_in_characters * sizeof(char_t);

  // +2 sentinels
  std::vector<char_t> result(size_in_characters + 2);
  stream.read(reinterpret_cast<char*>(&(result.data()[1])), size_in_bytes);
  stream.close();

  std::cout << "Finished reading file \"" << file_name << "\"." << std::endl;
  std::cout << "Size (w/o sentinels): "
            << "[" << size_in_characters << " characters] = "
            << ((size_in_bytes > 1023)
                    ? ("[" + std::to_string(size_in_bytes) + " bytes] = ")
                    : "")
            << "[" << to_SI_string(size_in_bytes) << "]" << std::endl;
  standardize(result);
  return result;
}