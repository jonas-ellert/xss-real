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

#include <cmath>
#include <executable/generator/pizzachili/data.hpp>
#include <experimental/filesystem>

static void head(const std::string path,
                 const std::string prefix_path,
                 const uint64_t length) {
  [[maybe_unused]] auto code = system(
      ("head -c " + std::to_string(length) + " " + path + " > " + prefix_path)
          .c_str());
}

static void prefixes_pc(const std::string dir) {
  for (uint64_t i = 0; i < pc_data.instances; ++i) {

    const std::string path = dir + pc_data.file_list[i];

    if (!std::experimental::filesystem::exists(path)) {
      std::cout << "File \"" << path << "\" does not exist. Skipping."
                << std::endl;
    } else {
      auto size = std::experimental::filesystem::file_size(path);
      std::cout << "Building prefixes of \"" << path << "\"." << std::endl
                << "Original Size: " << size << std::endl;

      while (size > 1024ULL * 1024) {
        const uint64_t lengthlog = (uint64_t) std::ceil(std::log2(size >> 1));
        size = 1ULL << lengthlog;
        const std::string prefix_path = path + "." + std::to_string(size);
        if (std::experimental::filesystem::exists(prefix_path)) {
          std::cout << "File \"" << prefix_path
                    << "\" already exists. Skipping." << std::endl;
        } else {
          std::cout << "Writing prefix \"" << prefix_path << "\"..."
                    << std::endl;
          head(path, prefix_path, size);
        }
      }
    }
  }
}