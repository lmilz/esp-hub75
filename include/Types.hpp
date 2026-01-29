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
 */
enum class ScanRate : uint8_t {:
    Scan1_4 = 4,   ///< 1/4 scan  - 2 address bits (A,B)
    Scan1_8 = 8,   ///< 1/8 scan  - 3 address bits (A,B,C)
    Scan1_16 = 16, ///< 1/16 scan - 4 address bits (A,B,C,D)
    Scan1_32 = 32  ///< 1/32 scan - 5 address bits (A,B,C,D,E)
};

constexpr uint8_t address_bits(ScanRate rate) {
    switch (rate) {
        case ScanRate::Scan1_4:  return 2;
        case ScanRate::Scan1_8:  return 3;
        case ScanRate::Scan1_16: return 4;
        case ScanRate::Scan1_32: return 5;
    }

    return 0;
}

struct Color {
    uint16_t raw{0};

    constexpr Color() = default;
    constexpr explicit Color(uint16_t rgb565) : raw(rgb565) {}

    constexpr Color(uint8_t r, uint8_t g, uint8_t b) : raw(static_cast<uint16_t>(((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F)) {}

    [[nodiscard]] constexpr uint8_t r() const { return (raw >> 11) & 0x1F; }
    [[nodiscard]] constexpr uint8_t g() const { return (raw >> 5) & 0x3F; }
    [[nodiscard]] constexpr uint8_t b() const { return raw & 0x1F; }

    [[nodiscard]] constexpr Color with_brightness(uint8_t brightness) const {
        return Color(
                static_cast<uint8_t>((r() * brightness) >> 8),
                static_cast<uint8_t>((g() * brightness) >> 8),
                static_cast<uint8_t>((b() * brightness) >> 8));
    }

    [[nodiscard]] constexpr Color blend(Color other, uint8_t alpha) const {
        const uint8_t inv = 255 - alpha;
        return Color(
                static_cast<uint8_t>((r() * inv + other.r() * alpha) >> 8),
                static_cast<uint8_t>((b() * inv + other.b() * alpha) >> 8),
                static_cast<uint8_t>((g() * inv + other.g() * alpha) >> 8));
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
