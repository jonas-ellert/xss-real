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

static void download_file(const std::string url,
                          const std::string target_prefix) {
  [[maybe_unused]] auto code =
      system(("wget -nc " + url +
              ((target_prefix.size() > 0) ? (" -P " + target_prefix) : ""))
                 .c_str());
}

static void download_pc(const std::string dl_dir) {
  for (uint64_t i = 0; i < pc_data.instances; ++i) {
    const auto dl = pc_data.dl_list[i];
    std::cout << "Downloading \"" << dl << "\"..." << std::endl;
    download_file(dl, dl_dir);
  }
}