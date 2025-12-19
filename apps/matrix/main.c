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
 * @file main.c
 * @brief Matrix rain effect application
 *
 * Classic "falling code" animation from The Matrix
 *
 * Build: idf.py -DAPP=matrix build flash monitor
 */

#include <stdio.h>
#include "hub75.h"

/* ====================================================
 * Configuration
 * ====================================================
 */

#define STREAM_COUNT      25 // Number of falling streams
#define STREAM_MIN_LENGTH 5  // Minimum trail length
#define STREAM_MAX_LENGTH 20 // Maximum trail length
#define STREAM_MIN_SPEED  1  // Fastest (frames per step)
#define STREAM_MAX_SPEED  3  // Slowest
#define SPAWN_CHANCE      4  // 1 in N chances per frame                          

/* ====================================================
 * Types
 * ====================================================
 */

typedef struct {
    bool    active;
    uint8_t column;
    uint8_t head_row;
    uint8_t length;
    uint8_t speed;
    uint8_t frame_counter;
} stream_t;

static stream_t streams[STREAM_COUNT];

/* ====================================================
 * Stream Management
 * ====================================================
 */


/* ====================================================
 * Application
 * ====================================================
 */

void app_main() {
    hub75_config_t config = HUB75_CONFIG_DEFAULT();

    hub75_handle_t panel = hub75_init(&config);
    if (panel == NULL) {
        return;
    }

    // Main loop
    while (true) {
        hub75_gfx_clear(panel, HUB75_GFX_RED);
        hub75_refresh(panel);
    }
}
