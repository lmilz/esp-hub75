// MIT License
//
// Copyright (c) 2025 Lars Milz
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
 * @file: hub75.h
 * @brief HUB75 LED Matrix Panel Driver API
 *
 * Low-Level driver for HUB75 RGB LED matrix panels.
 * Supports panels multiple scan rates.
 *
 */

#ifndef HUB75_H
#define HUB75_H

// Includes
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* ===================================================
 * Predefined Colors (RGB565)
 * ===================================================
 */

#define HUB75_GFX_BLACK    0x0000
#define HUB75_GFX_WHITE    0xFFFF
#define HUB75_GFX_RED      0xF800
#define HUB75_GFX_GREEN    0x07E0
#define HUB75_GFX_BLUE     0x001F


/* ===================================================
 * Scan Rate Definitions
 * ===================================================
 */

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
 * | 1/32      | 5 (A,B,C,D,E) | height/32     | 64y64          |
 */

typedef enum {
    HUB75_SCAN_1_4 = 4,   ///< 1/4 scan  - 2 address bits (A,B)
    HUB75_SCAN_1_8 = 8,   ///< 1/8 scan  - 3 address bits (A,B,C)
    HUB75_SCAN_1_16 = 16, ///< 1/16 scan - 4 address bits (A,B,C,D)
    HUB75_SCAN_1_32 = 32  ///< 1/32 scan - 5 address bits (A,B,C,D,E)
} hub75_scan_rate_t;

/* ===================================================
 * Type Definitions
 * ===================================================
 */

/**
 * @brief Default pin configuration for common ESP32 wiring
 */
#define HUB75_PINS_DEFAULT() { \
    .r1 = 25, .g1 = 26, .b1 = 27, \
    .r2 = 14, .g2 = 12, .b2 = 13, \
    .addr_a = 22, .addr_b = 23, .addr_c = 5, \
    .addr_d = 33, .addr_e = 32, \
    .clk = 18, .lat = 19, .oe = 21 \
}

/**
 * @brief Default configuration for 64x64 panel
 */
#define HUB75_CONFIG_DEFAULT() { \
    .pins = HUB75_PINS_DEFAULT(), \
    .width = 64, \
    .height = 64, \
    .scan_rate = HUB75_SCAN_1_32, \
    .row_time_us = 120 \
}

/**
 * @brief Driver handle
 */
typedef struct hub75_driver* hub75_handle_t;

/**
 * @brief GPIO pin configuration
 *
 * Set unused address pins to HUB75_PIN_UNUSED.
 */
typedef struct {
    uint8_t r1, g1, b1; ///< RGB data - upper half
    uint8_t r2, g2, b2; ///< RGB data - lower half
    uint8_t addr_a;     ///< Row address bit 0 (all scan rates)
    uint8_t addr_b;     ///< Row address bit 1 (all scan rates)
    uint8_t addr_c;     ///< Row address bit 2 (1/8, 1/17, 1/32)
    uint8_t addr_d;     ///< Row address bit 3 (1/16, 1/32)
    uint8_t addr_e;     ///< Row address bit 4 (1/16, 1/32)
    uint8_t clk;        ///< Pixel clock
    uint8_t lat;        ///< Latch
    uint8_t oe;         ///< Output Enable
} hub75_pins_t;

/**
 * @brief Panel configuration
 */
typedef struct {
    hub75_pins_t      pins;        ///< GPIO pin mapping
    uint8_t           width;       ///< Panel width
    uint8_t           height;      ///< Panel height
    hub75_scan_rate_t scan_rate;   ///< Panel scan rate
    uint8_t           row_time_us; ///< Row display duration in us
} hub75_config_t;

/**
 * @brief Marker for unused GPIO pins
 */
#define HUB75_PIN_UNUSED 0xFF

/* ===================================================
 * HUB75 and Framebuffer API
 * ===================================================
 */

/**
 * @brief Initialize the HUB75 driver
 *
 * Configures GPIO pins based on scan rate and allocates the framebuffer.
 * Validates that panel dimensions match the scan rate.
 *
 * @param config panel configuration
 * @return Handle on success, NULL on failure (invalid config or allocation error)
 */
hub75_handle_t hub75_init(const hub75_config_t* config);

/**
 * @brief Release the driver and free resources
 * @param handle Driver handle
 */
void hub75_release(hub75_handle_t handle);

/**
 * @brief Refresh the display
 *
 * Scans through all rows once according to the configuration scan rate.
 * Must be called repeatedly in a loop for a stable image.
 *
 * @param handle Driver handle
 */
void hub75_refresh(hub75_handle_t handle);

/**
 * @brief Enable or disable display output
 * @param handle Driver handle
 * @param enabled true = display on, false = display blanked
 */
void hub75_set_enabled(hub75_handle_t handle, bool enabled);

/* ===================================================
 * HUB75 Graphics API
 * ===================================================
 */

/**
 * @brief Clear entire canvas to a color
 * @param panel HUB75 driver panel
 * @param color Fill color (RGB565)
 */
void hub75_gfx_clear(hub75_handle_t panel, uint16_t color);

/**
 * @brief Set a single pixel
 * @param panel HUB75 driver handle
 * @param x X coordinate
 * @param y Y coordinate
 * @param color Color (RGB565)
 */
void hub75_gfx_set_pixel(hub75_handle_t panel, uint8_t x, uint8_t y, uint16_t color);
 
#endif /* HUB75_H */
