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
 * @file: Types.hpp
 * @brief Fundamental type definitions for the HUB75 LED panel driver.
 *
 * This header defines the core vocabulary types shared across all layers of the
 * hub7t driver stack. It is itentionally free of platform-specific dependencies
 * so that higher-level components (Framebuffer, Driver, GPIO abstractions) can
 * be compiled and unit-tested on any hosted C++20 toolchain.
 *
 * @par Provided types
 * | Type        | Purpose                                              |
 * |-------------|------------------------------------------------------|
 * | ScanRate    | Panel multiplexing ratio                             |
 * | Color       | Pixel color in RGB565 encoding                       |
 * | Point       | 2D pixel coordinate on the panel                     |
 * | PinConfig   | Mapping of logical HUB75 signals to GPIO pin numbers |
 * | PanelConfig | Compile-time panel geometry and run-time tuning      |
 *
 * @par Design rationale
 * - RGB565 is chosen because HUB75 panels are inherently limited to 5/6/5-bit
 * color depth per sub-pixel. Storing the native format avoids per-pixel
 * conversion during scan-out and halves the framebuffer size compared to
 * RGB888.
 * - uint8_t coordinates suffice because commercially available HUB75 panels do
 * not exceed 256 pixels in either dimension.
 * - PanelConfig is a template so that framebuffer sizes, scan-row counts, and
 * address-bit requirements are resolved at compile-time with zero run-time
 * overhead.
 */

#ifndef HUB75_TYPES_H
#define HUB75_TYPES_H

#include <cstddef>
#include <cstdint>

namespace hub75 {
/**
 * @brief Multiplexing ratio of a HUB75 LED panel.
 *
 * A HUB75 panel drives only a fraction of its row at any given instant.
 * The scan rate expresses that fraction: a 1/16 panel activates height/16 rows
 * simultaneouosly and cycles through 16 groups to complete one frame.
 *
 * The numeric value of each enumerator equals the denominator of the ration,
 * which is also the number of row-groups the panel must cycle through per
 * frame.
 *
 * | Scan Rate | Enum Value | Address Bits  | Rows per Scan | Typical panels |
 * |-----------|------------|---------------|---------------|----------------|
 * | 1/4       | 4          | 2 (A,B)       | height/4      | 16x16          |
 * | 1/8       | 8          | 3 (A,B,C)     | height/8      | 32x16          |
 * | 1/16      | 16         | 4 (A,B,C,D)   | height/16     | 32x32, 64x32   |
 * | 1/32      | 32         | 5 (A,B,C,D,E) | height/32     | 64x64          |
 *
 * @see address_bits()
 */
enum class ScanRate : uint8_t {
  Scan1_4 = 4,   ///< 1/4 scan  - 2 address bits (A,B)
  Scan1_8 = 8,   ///< 1/8 scan  - 3 address bits (A,B,C)
  Scan1_16 = 16, ///< 1/16 scan - 4 address bits (A,B,C,D)
  Scan1_32 = 32  ///< 1/32 scan - 5 address bits (A,B,C,D,E)
};

/**
 * @brief Returns the number of row-address GPIO pins required for a given scan
 * rate.
 *
 * The row-address pins (commonly labelled A..E on the HUB75 connector) select
 * which row-group is currently being driven.
 *
 * @param rate The scan rate of the panel
 * @return Number of address bits [2-5]
 *
 * @relates ScanRate
 */
constexpr uint8_t address_bits(ScanRate rate) {
  switch (rate) {
  case ScanRate::Scan1_4:
    return 2;
  case ScanRate::Scan1_8:
    return 3;
  case ScanRate::Scan1_16:
    return 4;
  case ScanRate::Scan1_32:
    return 5;
  }

  __builtin_unreachable();
}

/**
 * @brief RGB565 color representation for HUB75 panels
 *
 * Stores color as a 16-bit value in RGB565 format
 * - Bits 15-11: Red   (5 bit, 0-31)
 * - Bits 10-5:  Green (6 bit, 0-63)
 * - Bits  4-0:  Blue  (5 bit, 0-31)
 *
 * Green receives an extra bit because the human eye is most sensitive to green
 * wavelengths.
 *
 * @par Construction
 * - Default-constructed Color is black.
 * - Use the explicit constructor for a raw RGB565 value.
 * - Use from_rgb888() to convert from standard 8-bit-per-chanel RGB.
 *
 * @par Arithmetic
 * All color math (brightness, blending) operates in RGB565 space.
 * This avoids conversion overhead but comes at reduced precision.
 * For best visual quality in RGB888 externally and convert the result via
 * from_rgb888()
 *
 */
struct Color {
  uint16_t raw{0}; ///< Packed RGB565 value

  /** @brief Default constructor. Initializes to black. */
  constexpr Color() = default;

  /**
   * @brief Construct from a raw RGB565 value.
   * @param rgb565 Packed 16-bit cikir (R[15:11] | G[10:5] | B[4:0]).
   */
  constexpr explicit Color(uint16_t rgb565) : raw(rgb565) {}

  /**
   * @brief Convert an 8-bit-per-channel RGB triplet ro RGB565.
   *
   * Each channel is truncated (not rounded) to the target bit width:
   * - Red:   8 -> 5 bit (>> 3)
   * - Green: 8 -> 6 bit (>> 2)
   * - Blue:  8 -> 5 bit (>> 3)
   *
   * @param r Red channel (0-255).
   * @param g Green channel (0-255).
   * @param b Blue channel (0-255).
   * @return The corresponding RGB565 color.
   */
  static constexpr Color from_rgb888(uint8_t r, uint8_t g, uint8_t b) {
    return Color(
        static_cast<uint16_t>(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)));
  }

  [[nodiscard]] constexpr uint8_t r() const { return (raw >> 11) & 0x1F; }
  [[nodiscard]] constexpr uint8_t g() const { return (raw >> 5) & 0x3F; }
  [[nodiscard]] constexpr uint8_t b() const { return raw & 0x1F; }

  /**
   * @brief Returns a brightness-adjusted copy of this color
   *
   * @param brightness Brightness scale factor (0 = off, 255 = full)
   * @return New color with each channel scaled by brightness / 255
   */
  [[nodiscard]] constexpr Color with_brightness(uint8_t brightness) const {
    const uint8_t r5 = static_cast<uint8_t>((r() * brightness) / 255);
    const uint8_t g6 = static_cast<uint8_t>((g() * brightness) / 255);
    const uint8_t b5 = static_cast<uint8_t>((b() * brightness) / 255);
    return Color(static_cast<uint16_t>((r5 << 11) | (g6 << 5) | b5));
  }

  /**
   * @brief Linearly interpolates between this color and another
   *
   * @param other The color to blend towards
   * @param alpha Blend factor (0 = this color, 255 = other)
   * @return The blended color
   */
  [[nodiscard]] constexpr Color blend(Color other, uint8_t alpha) const {
    const uint8_t inv = 255 - alpha;
    const uint8_t r5 =
        static_cast<uint8_t>((r() * inv + other.r() * alpha) / 255);
    const uint8_t g6 =
        static_cast<uint8_t>((g() * inv + other.g() * alpha) / 255);
    const uint8_t b5 =
        static_cast<uint8_t>((b() * inv + other.b() * alpha) / 255);
    return Color(static_cast<uint16_t>((r5 << 11) | (g6 << 5) | b5));
  }

  static constexpr Color black() { return Color(0x0000); }
  static constexpr Color white() { return Color(0xFFFF); }
  static constexpr Color red() { return Color(0xF800); }
  static constexpr Color green() { return Color(0x07E0); }
  static constexpr Color blue() { return Color(0x001F); }

  constexpr bool operator==(const Color &) const = default;
};

/**
 * @brief 2D pixel coordinate on a HUB75 panel
 *
 * Uses uint8_t since typical HUB75 panels do not exceed 256 pixels per axis
 */
struct Point {
  uint8_t x{0};
  uint8_t y{0};

  constexpr Point() = default;
  constexpr Point(uint8_t x, uint8_t y) : x(x), y(y) {}

  /**
   * @brief Linear framebuffer index for a given panel width
   *
   * @param width Panel width
   * @returns Index in the framebuffer
   */
  [[nodiscard]] constexpr uint16_t index(uint8_t width) const {
    return static_cast<uint16_t>(static_cast<uint16_t>(y) * width + x);
  }

  constexpr bool operator==(const Point &) const = default;
};

/**
 * @brief GPIO pin assignment for a HUB75 panel
 *
 * Maps logical HUB75 signals to GPIO pins.
 * Set unused address pins to UNUSED.
 */
struct PinConfig {
  uint8_t r1, g1, b1;                             ///< RGB upper half
  uint8_t r2, g2, b2;                             ///< RGB lower half
  uint8_t addr_a, addr_b, addr_c, addr_d, addr_e; ///< Row address
  uint8_t oe, lat, clk;                           ///< Control

  static constexpr uint8_t UNUSED = 0xFF;
};

/**
 * @brief Default GPIO mapping for my specific wiring
 */
inline constexpr PinConfig DEFAULT_PINS = {.r1 = 25,
                                           .g1 = 26,
                                           .b1 = 27,
                                           .r2 = 14,
                                           .g2 = 12,
                                           .b2 = 13,
                                           .addr_a = 22,
                                           .addr_b = 23,
                                           .addr_c = 5,
                                           .addr_d = 33,
                                           .addr_e = 32,
                                           .oe = 21,
                                           .lat = 19,
                                           .clk = 18};

/**
 * @brief Panel configuration
 *
 * @tparam W    Panel width in pixels
 * @tparam H    Panel height in pixels
 * @tparam Scan Scan rate (default: 1/16)
 */
template <uint8_t W, uint8_t H, ScanRate Scan = ScanRate::Scan1_16>
struct PanelConfig {
  static constexpr uint8_t width = W;
  static constexpr uint8_t height = H;
  static constexpr ScanRate scan_rate = Scan;
  static constexpr uint8_t addr_bits = address_bits(Scan);
  static constexpr uint8_t scan_rows = H / 2;
  static constexpr size_t pixel_count = W * H;
  static constexpr size_t framebuffer_bytes = pixel_count * sizeof(Color);
  uint8_t row_time_us{120};
  PinConfig pins{DEFAULT_PINS};
};

/** @brief Preset for 64x64 panels with 1/32 scan rate */
using Panel64x64 = PanelConfig<64, 64, ScanRate::Scan1_32>;
/** @brief Preset for 32x32 panels with 1/16 scan rate */
using Panel32x32 = PanelConfig<32, 32, ScanRate::Scan1_16>;

} // namespace hub75

#endif /* HUB75_TYPES_H */
