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

template <uint64_t bits>
using uint_t = typename std::conditional<
    (bits > 32),
    uint64_t,
    typename std::conditional<
        (bits > 16),
        uint32_t,
        typename std::conditional<(bits > 8), uint16_t, uint8_t>::type>::type>::
    type;

template <uint64_t bits>
using int_t = typename std::conditional<
    (bits > 32),
    int64_t,
    typename std::conditional<
        (bits > 16),
        int32_t,
        typename std::conditional<(bits > 8), int16_t, int8_t>::type>::type>::
    type;

template <uint64_t bytes>
using uint_bytes_t = uint_t<8 * bytes>;

template <uint64_t bytes>
using int_bytes_t = int_t<8 * bytes>;

static_assert(sizeof(uint_t<8>) == 1, "sanity check");
static_assert(sizeof(uint_t<16>) == 2, "sanity check");
static_assert(sizeof(uint_t<24>) == 4, "sanity check");
static_assert(sizeof(uint_t<32>) == 4, "sanity check");
static_assert(sizeof(uint_t<40>) == 8, "sanity check");
static_assert(sizeof(uint_t<48>) == 8, "sanity check");
static_assert(sizeof(uint_t<56>) == 8, "sanity check");
static_assert(sizeof(uint_t<64>) == 8, "sanity check");

static_assert(sizeof(uint_t<7>) == 1, "sanity check");
static_assert(sizeof(uint_t<15>) == 2, "sanity check");
static_assert(sizeof(uint_t<23>) == 4, "sanity check");
static_assert(sizeof(uint_t<31>) == 4, "sanity check");
static_assert(sizeof(uint_t<39>) == 8, "sanity check");
static_assert(sizeof(uint_t<47>) == 8, "sanity check");
static_assert(sizeof(uint_t<55>) == 8, "sanity check");
static_assert(sizeof(uint_t<63>) == 8, "sanity check");

static_assert(sizeof(uint_t<9>) == 2, "sanity check");
static_assert(sizeof(uint_t<17>) == 4, "sanity check");
static_assert(sizeof(uint_t<25>) == 4, "sanity check");
static_assert(sizeof(uint_t<33>) == 8, "sanity check");
static_assert(sizeof(uint_t<41>) == 8, "sanity check");
static_assert(sizeof(uint_t<49>) == 8, "sanity check");
static_assert(sizeof(uint_t<57>) == 8, "sanity check");