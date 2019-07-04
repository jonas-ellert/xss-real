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

#include <executable/generator/pizzachili/data.hpp>
#include <experimental/filesystem>

static void extract_file(const std::string source, const std::string dest) {
  [[maybe_unused]] auto code =
      system(("yes n | gunzip -cv " + source + " > " + dest).c_str());
}

// static void move_file(const std::string source, const std::string dest) {
//  [[maybe_unused]]
//  auto code = system(("mv " + source + " " + dest).c_str());
//}

static void extract_pc(const std::string source_dir,
                       const std::string target_dir) {
  for (uint64_t i = 0; i < pc_data.instances; ++i) {

    const std::string source_path =
        source_dir + pc_data.file_list[i] + pc_data.archive_suffix;
    const std::string target_path = target_dir + pc_data.file_list[i];

    if (std::experimental::filesystem::exists(target_path)) {
      std::cout << "File \"" << target_path << "\" already exists. Skipping."
                << std::endl;
    } else {
      std::cout << "Extracting from/to:" << std::endl;
      std::cout << "    " << source_path << std::endl;
      std::cout << "    " << target_path << std::endl;
      extract_file(source_path, target_path);
    }
  }
}