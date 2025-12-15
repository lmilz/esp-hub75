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
 * @file: hub75_gfx.h
 * @brief Graphics API for HUB75 LED Matrix Panels
 *
 * Provides drawing primitives on top of the HUB75 driver.
 * All functions perform bounds checking automatically.
 *
 */

#ifndef HUB75_GFX_H
#define HUB75_GFX_H

// Includes
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hub75.h"

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
 * Graphics Context
 * ===================================================
 */

/**
 * @brief Graphics context structure
 *
 * Lightweight wrapper around the HUB75 driver.
 */

typedef struct {
} hub75_gfx_t;

/* ===================================================
 * HUB75 Graphics API
 * ===================================================
 */

/**
 * @brief Initialize a graphics context
 *
 * @param gfx Graphics context to initialize
 * @param panel HUB75 driver handle
 * @return true on success, false if panel is invalid
 */
bool hub75_gfx_init(hub75_gfx_t* gfx, hub75_handle_t panel);

/**
 * @brief Clear entire canvas to a color
 * @param gfx Graphics context
 * @param color Fill color (RGB565)
 */
void hub75_gfx_clear(hub75_gfx_t* gfx, uint16_t color);

/**
 * @brief Refresh the display
 *
 * Convenience wrapper for hub75_refresh()
 *
 * @param gfx Graphics context
 */
void hub75_gfx_refresh(hub75_gfx_t* gfx);

/**
 * @brief Set a single pixel
 * @param gfx Graphics context
 * @param x X coordinate
 * @param y Y coordinate
 * @param color Color (RGB565)
 */
void hub75_gfx_set_pixel(hub75_gfx_t* gfx, uint8_t x, uint8_t y, uint16_t color);
 
#endif /* HUB75_GFX_H */
