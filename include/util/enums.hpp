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

enum ds_direction_flag { next, previous };

constexpr static ds_direction_flag NEXT = ds_direction_flag::next;
constexpr static ds_direction_flag PREVIOUS = ds_direction_flag::previous;

constexpr static ds_direction_flag NEXT_AND_PREVIOUS[] = {NEXT, PREVIOUS};

namespace std {
inline static std::string to_string(const ds_direction_flag direction) {
  if (direction == NEXT)
    return "NEXT";
  if (direction == PREVIOUS)
    return "PREVIOUS";
  return "UNKNOWN_ALLOC_STRAT";
}
} // namespace std