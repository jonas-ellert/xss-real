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
#include <executable/generator/pizzachili/download.hpp>
#include <executable/generator/pizzachili/extract.hpp>
#include <executable/generator/pizzachili/prefixes.hpp>
#include <util/common.hpp>
#include <util/file_util.hpp>

static void gen_pizza_chili(const std::string target_dir, bool prefixes) {

  const std::string dl_dir = target_dir + "download/";

  if (mkdir(dl_dir) != 0) {
    std::cerr << "Could not create target directory \"" << dl_dir << "\"."
              << std::endl;
    std::cerr << "Skipping pizza chili strings." << std::endl;
    return;
  }

  std::cout << "Downloading pizza chili text collection to directory:"
            << std::endl
            << "    " << dl_dir << std::endl;

  download_pc(dl_dir);
  extract_pc(dl_dir, target_dir);
  if (prefixes)
    prefixes_pc(target_dir);
}