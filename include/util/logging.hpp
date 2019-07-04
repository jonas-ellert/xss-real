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

#include <sstream>

class atomic_out {
  std::ostringstream stream;

public:
  template <typename in_type>
  atomic_out& operator<<(const in_type& in) {
    stream << in;
    return *this;
  }
  ~atomic_out() {
    std::cout << stream.str() << std::flush;
  }
};

template <typename bps_type>
inline static std::string bps_to_string(const bps_type& bps,
                                        const uint64_t start,
                                        const uint64_t length,
                                        bool ansi_color = false) {
  constexpr static auto o_ansi = "\033[1;32m(\033[0m";
  constexpr static auto c_ansi = "\033[1;31m)\033[0m";
  constexpr static auto o_plain = "(";
  constexpr static auto c_plain = ")";

  const auto& o = ansi_color ? o_ansi : o_plain;
  const auto& c = ansi_color ? c_ansi : c_plain;

  std::stringstream result;
  for (uint64_t i = start; i < start + length; ++i) {
    if (bps[i])
      result << o;
    else
      result << c;
  }
  return result.str();
}