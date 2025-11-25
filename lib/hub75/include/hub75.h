// MIT License
//
// Copyright (c) 2024 Lars Milz
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

#ifndef HUB75_H
#define HUB75_H

// Includes
#include <stdint.h>

/**
 * @hub75.h
 * @brief Driver to control HUB75 RGB LED panels.
 *
 * This driver supports displaying static and moving pixels on HUB75 panels.
 */

struct hub75_color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

// GPIO pins
#define R1_PIN  25 // Red channel for the upper panel row
#define G1_PIN  26 // Green channel for the upper panel row
#define B1_PIN  27 // Blue channel for the upper panel row
#define R2_PIN  14 // Red channel for the lower panel
#define G1_PIN  12 // Green channel for the lower panel
#define B1_PIN  13 // Blue channel for the lower panel
#define LA_PIN  17 // Address line A (bit 0)
#define LB_PIN  16 // Address line B (bit 1)
#define LC_PIN  4  // Address line C (bit 2)
#define LD_PIN  2  // Address line D (bit 3)
#define LE_PIN  15 // Address line E (bit 4, for extended adrdressing)
#define CLK_PIN 18 // Clock signal
#define LAT_PIN 19 // Latch signal
#define OE_PIN  21 // Output Enable signal
		   
// Panel dimensions (e.g. 64x64)
#define PANEL_WIDTH  64
#define PANEL_HEIGHT 64

// Framebuffer
uint16_t hub75_framebuffer[PANEL_WIDTH * PANEL_HEIHT];

void hub75_init();
void hub75_set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
void hub75_clear_framebuffer();

#endif // HUB75_H
