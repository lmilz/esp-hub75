
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
 * @file hub75.c
 * @brief HUB75 LED matrix panel driver implementation
 */

#include <driver/gpio.h>
#include <esp_rom_sys.h>
#include <esp_log.h>
#include <stdlib.h>
#include "hub75.h"

/* ====================================================
 * Private Types and Functions
 * ====================================================
 */

static const char* TAG = "HUB75";

struct hub75_driver {
    hub75_pins_t      pins;
    uint8_t           width;
    uint8_t           height;
    hub75_scan_rate_t scan_rate;
    uint8_t           addr_bits;
    uint8_t           scan_rows;
    uint8_t           row_time_us;
    uint16_t*         framebuffer;
    bool              enabled;
};

static inline uint8_t hub75_get_address_bits(hub75_scan_rate_t scan_rate) {
    switch (scan_rate) {
        case HUB75_SCAN_1_4:  return 2;
        case HUB75_SCAN_1_8:  return 3;
        case HUB75_SCAN_1_16: return 4;
        case HUB75_SCAN_1_32: return 5;
        default:              return 0;
    }
}

/**
 * @brief Set row address with variable number of bits
 */
static inline void set_row_address(const struct hub75_driver* drv, uint8_t row) {
    const hub75_pins_t* pins = &drv->pins;

    // Always set A and B
    gpio_set_level(pins->addr_a,  row       & 0x01);
    gpio_set_level(pins->addr_b, (row >> 1) & 0x01);

    // C for scan rates 1/8, 1/16 and 1/32
    if (drv->addr_bits >= 3 && pins->addr_c != HUB75_PIN_UNUSED)
        gpio_set_level(pins->addr_c, (row >> 2) & 0x01);

    // D for scan rates 1/16 and 1/32
    if (drv->addr_bits >= 4 && pins->addr_d != HUB75_PIN_UNUSED)
        gpio_set_level(pins->addr_d, (row >> 3) & 0x01);

    // E for scan rate 1/32 only
    if (drv->addr_bits >= 5 && pins->addr_e != HUB75_PIN_UNUSED)
        gpio_set_level(pins->addr_e, (row >> 4) & 0x01);
}

static void init_gpio(const struct hub75_driver* drv) {
    const hub75_pins_t* pins = &drv->pins;

    uint64_t pin_mask = 
      (1ULL << pins->r1)  | (1ULL << pins->g1)  | (1ULL << pins->b1) |
      (1ULL << pins->r2)  | (1ULL << pins->g2)  | (1ULL << pins->b2) |
      (1ULL << pins->clk) | (1ULL << pins->lat) | (1ULL << pins->oe);

    // Add address pins based on scan rate
    pin_mask |= (1ULL << pins->addr_a);
    pin_mask |= (1ULL << pins->addr_b);

    if (drv->addr_bits >= 3 && pins->addr_c != HUB75_PIN_UNUSED)
        pin_mask |= (1ULL << pins->addr_c);
    if (drv->addr_bits >= 4 && pins->addr_d != HUB75_PIN_UNUSED)
        pin_mask |= (1ULL << pins->addr_d);
    if (drv->addr_bits >= 5 && pins->addr_e != HUB75_PIN_UNUSED)
        pin_mask |= (1ULL << pins->addr_e);

    gpio_config_t config = {
        .pin_bit_mask = pin_mask,
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE
    };
    gpio_config(&config);

    // Initial states
    gpio_set_level(pins->oe, 1); // Display off
    gpio_set_level(pins->lat, 0); // Latch inactive
    gpio_set_level(pins->clk, 0); // Clock low
                                  
    // Clear data lines
    gpio_set_level(pins->r1, 0);
    gpio_set_level(pins->g1, 0);
    gpio_set_level(pins->b1, 0);
    gpio_set_level(pins->r2, 0);
    gpio_set_level(pins->g2, 0);
    gpio_set_level(pins->b2, 0);

    set_row_address(drv, 0);
}

/* ====================================================
 * Public API - HUB75 Driver
 * ====================================================
 */

hub75_handle_t hub75_init(const hub75_config_t* config) {
    // TODO: valid config
    
    // Init driver
    struct hub75_driver* drv = malloc(sizeof(struct hub75_driver));
    if (drv == NULL) {
        ESP_LOGE(TAG, "Failed to allocate driver structure");
        return NULL;
    }

    // Init framebuffer
    size_t fb_size = config->width*config->height*sizeof(uint16_t);
    drv->framebuffer = malloc(fb_size);
    if (drv->framebuffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate framebuffer (%d bytes)", fb_size);
        free(drv);
        return NULL;
    }

    memcpy(&drv->pins, &config->pins, sizeof(hub75_pins_t));
    drv->width       = config->width;
    drv->height      = config->height;
    drv->scan_rate   = config->scan_rate;
    drv->addr_bits   = hub75_get_address_bits(config->scan_rate);
    drv->scan_rows   = 0.5*config->height;
    drv->row_time_us = config->row_time_us;
    drv->enabled     = true;

    memset(drv->framebuffer, 0, fb_size);
    init_gpio(drv);

    ESP_LOGI(TAG, "Initialized %d%x%d panel, 1/%d scan", config->width, config->height, config->scan_rate);

    return drv;
}

void hub75_release(hub75_handle_t handle) {
    if (handle == NULL)
        return;

    gpio_set_level(handle->pins.oe, 1); // Disable display
    free(handle->framebuffer);
    free(handle);
}

void hub75_refresh(hub75_handle_t handle) {
    if (handle == NULL)
        return;
 
    const hub75_pins_t* pins = &handle->pins;

    for (uint8_t row = 0; row < handle->scan_rows; row++) {
        gpio_set_level(pins->oe, 1);
        set_row_address(handle, row);

        // Determine framebuffer row indices
        // Upper half: rows 0 to (height/2 - 1)
        // Lower half: row (height/2) to (height - 1)
        const uint16_t* row_upper = &handle->framebuffer[row * handle->width];
        const uint16_t* row_lower = &handle->framebuffer[(row + handle->scan_rows) * handle->width];

        // Shift out all columns
        for (uint8_t col = 0; col < handle->width; col++) {

            // Color mode
            gpio_set_level(pins->r1, ((row_upper[col] >> 11) & 0x1F) != 0);
            gpio_set_level(pins->g1, ((row_upper[col] >> 5)  & 0x3F) != 0);
            gpio_set_level(pins->b1,  (row_upper[col]        & 0x1F) != 0);
            
            gpio_set_level(pins->r2, ((row_lower[col] >> 11) & 0x1F) != 0);
            gpio_set_level(pins->g2, ((row_lower[col] >> 5)  & 0x3F) != 0);
            gpio_set_level(pins->b2,  (row_lower[col]        & 0x1F) != 0);

            // Clock pulse
            gpio_set_level(pins->clk, 1);
            gpio_set_level(pins->clk, 0);
        }

        // Latch and enable
        gpio_set_level(pins->lat, 1);
        gpio_set_level(pins->lat, 0);

        if (handle->enabled) {
          gpio_set_level(pins->oe, 0);
          esp_rom_delay_us(handle->row_time_us);
        }
    }
}

void hub75_set_enabled(hub75_handle_t handle, bool enabled) {
    if (handle == NULL)
        return;
  
    handle->enabled = enabled;
    if (!enabled)
        gpio_set_level(handle->pins.oe, 1);
}
