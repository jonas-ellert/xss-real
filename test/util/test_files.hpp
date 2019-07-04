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

#include <experimental/filesystem>

const static std::string file_prefix = "test_data/";

static void mkdir_file_prefix() {
  constexpr static auto success = 0;
  const static std::string command = "mkdir -p " + file_prefix;
  auto error_code = system(command.c_str());
  if (error_code != success) {
    std::cout << "Could not locate or create directory '" << file_prefix << "'." << std::endl;
    EXPECT_EQ(error_code, success);
    std::abort();
  }
}

static std::string get_filename(const std::string instance_name, const uint64_t n = 0) {
  return file_prefix + instance_name + (n > 0 ? ("."  + std::to_string(n)) : "");
}

static std::vector<uint8_t> instance_from_file(const std::string instance_name, const uint64_t n) {
  const std::string filename = get_filename(instance_name, n);
  if (std::experimental::filesystem::exists(filename)) {
    std::cout << "[Found test instance " << filename << "]" << std::endl;

    std::ifstream is(filename);
    std::vector<uint8_t> result;

    is.seekg(0, std::ios::end);
    result.reserve(is.tellg());
    is.seekg(0, std::ios::beg);
    result.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());

    return result;
  }
  else {
    std::cout << "[Could not find test instance " << filename << "]" << std::endl;
    return std::vector<uint8_t>();
  }
}

template <typename container_type>
static void instance_to_file(const std::string instance_name, container_type &&instance, const uint64_t n) {
  mkdir_file_prefix();
  const std::string filename = get_filename(instance_name, n);
  if (std::experimental::filesystem::exists(filename)) {
    std::cout << "[Cannot save instance; already exists: " << filename << "]" << std::endl;
    std::abort();
  }
  else {
    std::ofstream outFile(filename);
    for (const auto character : instance) outFile << character;
    std::cout << "[Saved instance " << filename << "]" << std::endl;
  }
}
