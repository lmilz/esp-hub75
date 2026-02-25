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
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <gtest/gtest.h>

#include "Framebuffer.hpp"
#include "Types.hpp"

using namespace hub75;

// === Framebuffer Tests ===

TEST(Framebuffer, DefaultIsBlack) {
  Framebuffer<4, 4> fb;
  EXPECT_EQ(fb.get(0, 0), Color::black());
  EXPECT_EQ(fb.get(4, 4), Color::black());
}

TEST(Framebuffer, SetAndGet) {
  Framebuffer<4, 4> fb;
  fb.set(1, 2, Color::red());
  EXPECT_EQ(fb.get(1, 2), Color::red());
  EXPECT_EQ(fb.get(0, 0), Color::black());
}

TEST(Framebuffer, SetWithPoint) {
  Framebuffer<4, 4> fb;
  fb.set(Point{2, 3}, Color::blue());
  EXPECT_EQ(fb.get(Point{2, 3}), Color::blue());
}

TEST(Framebuffer, Fill) {
  Framebuffer<4, 4> fb;
  fb.fill(Color::green());
  for (uint8_t y = 0; y < 4; ++y) {
    for (uint8_t x = 0; x < 4; ++x) {
      EXPECT_EQ(fb.get(x, y), Color::green());
    }
  }
}

TEST(Framebuffer, Clear) {
  Framebuffer<4, 4> fb;
  fb.fill(Color::red());
  fb.clear();
  for (uint8_t y = 0; y < 4; ++y) {
    for (uint8_t x = 0; x < 4; ++x) {
      EXPECT_EQ(fb.get(x, y), Color::black());
    }
  }
}

TEST(Framebuffer, DataPointerNotNull) {
  Framebuffer<4, 4> fb;
  EXPECT_NE(fb.data(), nullptr);
}

TEST(Framebuffer, DataReflectContent) {
  Framebuffer<4, 4> fb;
  fb.set(0, 0, Color::red());
  const Color *data = fb.data();
  EXPECT_EQ(data[0], Color::red());
}

TEST(Framebuffer, OutOfBoundsGet) {
  Framebuffer<4, 4> fb;
  fb.fill(Color::white());
  EXPECT_EQ(fb.get(4, 0), Color::black()); // x == Width
  EXPECT_EQ(fb.get(0, 4), Color::black()); // y == Height
  EXPECT_EQ(fb.get(255, 255), Color::black());
}

TEST(Framebuffer, OutOfBoundsSet) {
  Framebuffer<4, 4> fb;
  fb.set(Point{4, 0}, Color::blue());
  fb.set(Point{0, 4}, Color::red());

  EXPECT_EQ(fb.get(4, 0), Color::black()); // x == Width
  EXPECT_EQ(fb.get(0, 4), Color::black()); // y == Height
}
