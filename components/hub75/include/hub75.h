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
    hub75_pins_t      pins;      ///< GPIO pin mapping
    uint8_t           width;     ///< Panel width
    uint8_t           height;    ///< Panel height
    hub75_scan_rate_t scan_rate; ///< Panel scan rate
} hub75_config_t;

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
 * @brief Get direct pointer to the framebuffer
 *
 * The framebuffer uses RGB565 format (16 bits per pixel).
 * Layout: pixel[y*width+x]
 *
 * @param handle driver handle
 * @return Pointer to framebuffer, NULL if handle invalid
 */
uint16_t* hub75_get_framebuffer(hub75_handle_t handle);

/**
 * @brief Get framebuffer size in bytes
 * @param handle Driver handle
 * @return Size in bytes
 */
size_t hub75_get_framebuffer_size(hub75_handle_t handle);

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
 
#endif /* HUB75_H */
