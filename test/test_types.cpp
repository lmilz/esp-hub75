// MIT License
//
// Copyright (c) 2025-present Lars Milz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <gtest/gtest.h>

#include "Types.hpp"

using namespace hub75;

// === ScanRate Tests ===

TEST(ScanRate, AddressBitsMapping) {
    EXPECT_EQ(address_bits(ScanRate::Scan1_4),  2);
    EXPECT_EQ(address_bits(ScanRate::Scan1_8),  3);
    EXPECT_EQ(address_bits(ScanRate::Scan1_16), 4);
    EXPECT_EQ(address_bits(ScanRate::Scan1_32), 5);
}

// === Color Tests ===

TEST(Color, DefaultIsBlack) {
    Color c;
    EXPECT_EQ(c.raw, 0x0000);
}

TEST(Color, PredefinedColors) {
    EXPECT_EQ(Color::black().raw, 0x0000);
    EXPECT_EQ(Color::white().raw, 0xFFFF);
    EXPECT_EQ(Color::red().raw,   0xF800);
    EXPECT_EQ(Color::green().raw, 0x07E0);
    EXPECT_EQ(Color::blue().raw,  0x001F);
}

TEST(Color, ChannelExtraction) {
    Color red = Color::red();
    EXPECT_EQ(red.r(), 31);
    EXPECT_EQ(red.g(), 0);
    EXPECT_EQ(red.b(), 0);

    Color white = Color::white();
    EXPECT_EQ(white.r(), 31);
    EXPECT_EQ(white.g(), 63);
    EXPECT_EQ(white.b(), 31);
}

TEST(Color, FromRgb888) {
    Color c = Color::from_rgb888(31, 63, 31);
    EXPECT_EQ(c, Color::white());
}

TEST(Color, Equality) {
    EXPECT_EQ(Color::red(), Color::red());
    EXPECT_NE(Color::red(), Color::blue());
}

TEST(Color, BrightnessZeroIsBlack) {
    Color c = Color::white().with_brightness(0);
    EXPECT_EQ(c, Color::black());
}

TEST(Color, BlendExtremes) {
    Color result_a = Color::red().blend(Color::blue(), 0);
    EXPECT_EQ(result_a, Color::red());

    Color result_b = Color::red().blend(Color::blue(), 255);
    EXPECT_EQ(result_b, Color::blue());
}

// === Point Tests ===

TEST(Point, DefaultIsOrigin) {
    Point p;
    EXPECT_EQ(p.x, 0);
    EXPECT_EQ(p.y, 0);
}

TEST(Point, Construction) {
    Point p(10, 20);
    EXPECT_EQ(p.x, 10);
    EXPECT_EQ(p.y, 20);
}

TEST(Point, Index) {
    Point p(3, 2);
    EXPECT_EQ(p.index(64), 131); // 2 * 64 + 3 = 131
}

TEST(Point, IndexOrigin) {
    Point p;
    EXPECT_EQ(p.index(64), 0);
}

TEST(Point, Equality) {
    EXPECT_EQ(Point(1, 2), Point(1, 2));
    EXPECT_NE(Point(1, 2), Point(2, 1));
}
