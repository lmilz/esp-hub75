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

/**
 * @file: Types.hpp
 * @brief Type definition for HUB75 driver
 *
 * Provides fundamental types used throughtout the HUB75 driver:
 * - ScanRate: Panel multiplexing configuration
 * - Color: RGB565 color representation
 */

#ifndef HUB75_TYPES_H
#define HUB75_TYPES_H


// Includes
#include <cstdint>
#include <array>

namespace hub75 {
/**
 * @brief Panel scan rate
 *
 * The scan rate determines how the panel multiplexes rows.
 * Lower scan rates = more rows driven simultaneously (brighter but more pins).
 *
 * | Scan Rate | Address Bits  | Rows per Scan | Typical panels |
 * |-----------|---------------|---------------|----------------|
 * | 1/4       | 2 (A,B)       | height/4      | 16x16          |
 * | 1/8       | 3 (A,B,C)     | height/8      | 32x16          |
 * | 1/16      | 4 (A,B,C,D)   | height/16     | 32x32, 64x32   |
 * | 1/32      | 5 (A,B,C,D,E) | height/32     | 64x64          |
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
 * @brief Returns the number of row-adress pins required for a given scan rate
 *
 * @param rate The scan rate of the panel
 * @return Number of address bits (2-5)
 *
 * @relates ScanRate
 */
constexpr uint8_t address_bits(ScanRate rate) {
    switch (rate) {
        case ScanRate::Scan1_4:  return 2;
        case ScanRate::Scan1_8:  return 3;
        case ScanRate::Scan1_16: return 4;
        case ScanRate::Scan1_32: return 5;
    }

    return 0;
}

/**
 * @brief RGB565 color representation for HUB75 panels
 *
 * Stores color as a 16-bit value in RGBG565 format
 * - Bits 15-11: Red   (5 bit, 0-31)
 * - Bits 10-5:  Green (6 bit, 0-63)
 * - Bits  4-0:  Blue  (5 bit, 0-31) 
 *
 * @note All arithmetic (brightness, blending) operates in RGB565 space. For best precision, convert from RGB888 via from_rgb888().
 */
struct Color {
    uint16_t raw{0};

    constexpr Color() = default;
    constexpr explicit Color(uint16_t rgb565) : raw(rgb565) {}

    static constexpr Color from_rgb888(uint8_t r, uint8_t g, uint8_t b) {
        return Color(static_cast<uint16_t>(((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F)));
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
        return from_rgb888(
                static_cast<uint8_t>((r() * brightness) / 255),
                static_cast<uint8_t>((g() * brightness) / 255),
                static_cast<uint8_t>((b() * brightness) / 255));
    }

    /**
     * @brief Linearly interpolates between this color and another
     *
     * @param other The color to blend torwards
     * @param alpha Blend factor (0 = this color, 255 = other)
     * @return The blended color
     */
    [[nodiscard]] constexpr Color blend(Color other, uint8_t alpha) const {
        const uint8_t inv = 255 - alpha;
        return from_rgb888(
                static_cast<uint8_t>((r() * inv + other.r() * alpha) / 255),
                static_cast<uint8_t>((g() * inv + other.g() * alpha) / 255),
                static_cast<uint8_t>((b() * inv + other.b() * alpha) / 255));
    }

    static constexpr Color black() { return Color(0x0000); }
    static constexpr Color white() { return Color(0xFFFF); }
    static constexpr Color red()   { return Color(0xF800); } 
    static constexpr Color green() { return Color(0x07E0); }
    static constexpr Color blue()  { return Color(0x001F); }

    constexpr bool operator==(const Color&) const = default;
};
} // namespace hub75

#endif /* HUB75_TYPES_H */
