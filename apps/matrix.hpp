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
 * @file matrix.hpp
 * @brief Matrix rain effect application
 *
 * Classic "falling code" animation from The Matrix
 */

#ifndef APPS_MATRIX_HPP
#define APPS_MATRIX_HPP

#include <array>
#include <cstdlib>

#include "Framebuffer.hpp"
#include "Types.hpp"

template <uint8_t Width, uint8_t Height, uint8_t NumStreams>
class MatrixStream {
    struct Stream {
        bool    active{false};
        uint8_t column{0};
        uint8_t head_row{0};
        uint8_t length{0};
        uint8_t speed{0};
        uint8_t frame_counter{0};
    };

  public:
    MatrixStream() = default;

    void update(hub75::Framebuffer<Width, Height>& fb) {
        render(fb);
        spawn();
    }

  private:
    void render(hub75::Framebuffer<Width, Height>& fb) {
        for (uint8_t index = 0; index < NumStreams; index++) {
            if (advance(streams_[index])) {
                auto& s = streams_[index];
                for (uint8_t offset = 0; offset < s.length; offset++) {
                    if (offset > s.head_row) { break; }
                    uint8_t y = s.head_row - offset;

                    if (y < Height) {
                        fb.set(s.column, y, trail_color(offset, s.length));
                    }
                }
            }
        }
    }

    void spawn() {
        if (rand() % spawn_chance_ == 0) {
            for (uint8_t index = 0; index < NumStreams; index++) {
                if (!streams_[index].active) {
                    streams_[index].active        = true;
                    streams_[index].column        = rand() % Width;
                    streams_[index].head_row      = 0;
                    streams_[index].length        = min_length_ + (rand() % (max_length_ - min_length_ + 1));
                    streams_[index].speed         = min_speed_ + (rand() % (max_speed_ - min_speed_ + 1));
                    streams_[index].frame_counter = 0;
                    return;
                }
            }
        }
    }

    bool advance(Stream& s) {
        if (!s.active) { return false; }

        s.frame_counter++;
        if (s.frame_counter >= s.speed) {
            s.head_row++;
            s.frame_counter = 0;
        }

        if (s.head_row > s.length + Height) {
            s.active = false;
            return false;
        }

        return true;
    }

    static hub75::Color trail_color(uint8_t offset, uint8_t length) {
        if (offset == 0) {
            return hub75::Color::from_rgb888(180, 255, 180);
        }

        uint8_t green = static_cast<uint8_t>(255 - (offset * 255) / length);
        return hub75::Color::from_rgb888(0, green, 0);
    }

    std::array<Stream, NumStreams> streams_{};
    uint8_t min_length_{5};
    uint8_t max_length_{20};
    uint8_t min_speed_{1};
    uint8_t max_speed_{3};
    uint8_t spawn_chance_{4};
};

#endif /* APPS_MATRIX_HPP */
