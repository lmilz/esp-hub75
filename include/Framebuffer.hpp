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

/**
 * @file: Framebuffer.hpp
 * @brief Framebuffer
 *
 */

#ifndef HUB75_FRAMEBUFFER_H
#define HUB75_FRAMEBUFFER_H

// Includes
#include <algorithm>
#include <array>
#include <functional>
#include <optional>

#include "Types.hpp"

namespace hub75 {

class IFramebuffer {
public:
  virtual ~IFramebuffer() = default;

  [[nodiscard]] virtual const Color *data() const = 0;
};

template <uint8_t Width, uint8_t Height>
class Framebuffer : public IFramebuffer {
public:
  constexpr Framebuffer() = default;

  [[nodiscard]] constexpr Color get(Point p) const {
    if (auto idx = index(p))
      return pixels[*idx];
    return Color::black();
  }

  [[nodiscard]] constexpr Color get(uint8_t x, uint8_t y) {
    return get(Point{x, y});
  }

  constexpr Framebuffer &set(Point p, Color c) {
    if (auto idx = index(p))
      pixels[*idx] = c;
    return *this;
  }

  constexpr Framebuffer &set(uint8_t x, uint8_t y, Color c) {
    return set(Point{x, y}, c);
  }

  constexpr Framebuffer &fill(Color c) {
    pixels.fill(c);
    return *this;
  }

  constexpr Framebuffer &clear() { return fill(Color::black()); }

  [[nodiscard]] const Color *data() const override { return pixels.data(); }

private:
  static constexpr size_t SIZE = Width * Height;
  std::array<Color, SIZE> pixels{};

  [[nodiscard]] constexpr std::optional<size_t> index(Point p) const {
    if (p.x >= Width || p.y >= Height) {
      return std::nullopt;
    }
    return static_cast<size_t>(p.y * Width + p.x);
  }
};

template <uint8_t Width, uint8_t Height>
class DoubleFramebuffer : public IFramebuffer {
public:
  constexpr DoubleFramebuffer() = default;

  Framebuffer<Width, Height> &back() { return buffers_[back_index_]; }

  Framebuffer<Width, Height> &front() { return buffers_[1 - back_index_]; }

  const Framebuffer<Width, Height> &front() const {
    return buffers_[1 - back_index_];
  }

  [[nodiscard]] const Color *data() const override { return front().data(); }

  void swap() { back_index_ = 1 - back_index_; }

  constexpr DoubleFramebuffer &set(uint8_t x, uint8_t y, Color c) {
    back().set(x, y, c);
    return *this;
  }

  constexpr DoubleFramebuffer &fill(Color c) {
    back().fill(c);
    return *this;
  }

  constexpr DoubleFramebuffer &clear() {
    back().clear();
    return *this;
  }

private:
  std::array<Framebuffer<Width, Height>, 2> buffers_{};
  uint8_t back_index_{0};
};

} // namespace hub75

#endif /* HUB75_FRAMEBUFFER_H */
