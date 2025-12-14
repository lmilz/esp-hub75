
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

#include <driver/gpio.h>
#include "hub75.h"

void hub75_init() {
	// clear framebuffer
	for (uint16_t i = 0; i < PANEL_WIDTH * PANEL_HEIGHT; ++i) {
		hub75_framebuffer[i] = 0;
	}

	//set gpio
	gpio_config_t io_config = {
		.pin_bit_mask = (1ULL << R1_PIN) | (1ULL << G1_PIN) | (1ULL << B1_PIN) |
			        (1ULL << R2_PIN) | (1ULL << G2_PIN) | (1ULL << B2_PIN) |
				(1ULL << LA_PIN) | (1ULL << LB_PIN) | (1ULL << LC_PIN) |
				(1ULL << LD_PIN) | (1ULL << LE_PIN) | (1ULL << CLK_PIN) |
				(1ULL << LAT_PIN) | (OE_PIN << LE_PIN),
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE
		
	};
	gpio_config(&io_config);
}

void hub75_set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
	// Check bounds
	if (x >= PANEL_WIDTH || y >= PANEL_HEIGHT)
		return;
}

void hub75_clear_framebuffer() {
	for (uint16_t i = 0; i < PANEL_WIDTH * PANEL_HEIGHT; ++i) {
		hub75_framebuffer[i] = 0;
	}
}
