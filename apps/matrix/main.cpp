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
 * @file main.cpp
 * @brief Matrix rain effect application
 *
 * Classic "falling code" animation from The Matrix
 *
 * Build: idf.py -DAPP=matrix build flash monitor
 */

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "hub75.hpp"

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

static void stream_spawn(uint8_t panel_width) {
    for (uint8_t index = 0; index < STREAM_COUNT; index++) {
        if (!streams[index].active) {
            streams[index].active        = true;
            streams[index].column        = rand() % panel_width;
            streams[index].head_row      = 0;
            streams[index].length        = STREAM_MIN_LENGTH + (rand() % (STREAM_MAX_LENGTH - STREAM_MIN_LENGTH + 1));
            streams[index].speed         = STREAM_MIN_SPEED + (rand() % (STREAM_MAX_SPEED -STREAM_MIN_SPEED + 1));
            streams[index].frame_counter = 0;
        }
    }
}

static bool stream_update(stream_t* s, uint8_t panel_height) {
    bool should_render = false;

    if (s->active) {
        s->frame_counter++;

        if (s->frame_counter >= s->speed) {
            s->head_row++;
            s->frame_counter = 0;
        }

        if (s->head_row - s->length > panel_height) {
            s->active = false;
        } else {
            should_render = true;
        }
    }

    return should_render;
}

static void stream_render(hub75_handle_t panel, stream_t* s, uint8_t panel_height) {
    for (uint8_t offset = 0; offset < s->length; offset++) {
        uint8_t y = s->head_row - offset;

        if (y < panel_height) {
            // Head is bright white-green, tail fades to dark green
            uint8_t brightness;
            if (offset == 0) {
                brightness = 63;
            } else if (40 - offset > 1) {
                brightness = 40 - offset;
            } else {
                brightness = 1;
            }

            uint16_t color = HUB75_RGB565(0, brightness, 0);
            hub75_gfx_set_pixel(panel, s->column, y, color);
        }
    }
}

/* ====================================================
 * Application
 * ====================================================
 */

extern "C" void app_main() {
    hub75_config_t config = HUB75_CONFIG_DEFAULT();

    hub75_handle_t panel = hub75_init(&config);
    if (panel == nullptr) {
        return;
    }

    // init of streams
    memset(streams, 0, sizeof(streams));

    // Main loop
    while (true) {
        hub75_gfx_clear(panel, HUB75_GFX_BLACK);

        for (uint8_t index = 0; index < STREAM_COUNT; index++) {
            if (stream_update(&streams[index], 64))
                stream_render(panel, &streams[index], 64);
        }

        if (rand() % SPAWN_CHANCE == 0)
            stream_spawn(64);

        hub75_refresh(panel);
    }
}
