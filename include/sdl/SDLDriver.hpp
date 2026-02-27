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
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/**
 * @file: SDLDriver.hpp
 * @brief SDL Driver
 *
 */

#ifndef HUB75_SDLDRIVER_H
#define HUB75_SDLDRIVER_H

#include <SDL2/SDL.h>

#include <stdexcept>
#include <string>

#include "Framebuffer.hpp"

namespace hub75::SDL {

template <uint8_t Width, uint8_t Height>
class SDLDriver {
  public:
    explicit SDLDriver(const IFramebuffer& fb, int scale = 8,
                       const std::string& title = "HUB75 Simulator")
        : fb_(fb), scale_(scale) {
        // SDL initialization
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
        }

        window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   Width * scale_, Height * scale_, SDL_WINDOW_SHOWN);

        if (!window_) {
            SDL_Quit();
            throw std::runtime_error("Window creation failed");
        }

        renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);

        if (!renderer_) {
            SDL_DestroyWindow(window_);
            SDL_Quit();
            throw std::runtime_error("Renderer creation failed");
        }
    }

    ~SDLDriver() {
        if (renderer_) SDL_DestroyRenderer(renderer_);
        if (window_) SDL_DestroyWindow(window_);
        SDL_Quit();
    }

    void refresh() {
        if (!enabled_) return;

        const Color* pixels = fb_.data();

        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
        SDL_RenderClear(renderer_);

        for (uint8_t y = 0; y < Height; ++y) {
            for (uint8_t x = 0; x < Width; ++x) {
                const Color c = pixels[y * Width + x];

                const uint8_t r = (c.r() << 3) | (c.r() >> 2);
                const uint8_t g = (c.g() << 2) | (c.g() >> 4);
                const uint8_t b = (c.b() << 3) | (c.b() >> 2);

                SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
                SDL_Rect rect = {x * scale_, y * scale_, scale_ - scale_ / 2, scale_ - scale_ / 2};
                SDL_RenderFillRect(renderer_, &rect);
            }
        }

        SDL_RenderPresent(renderer_);
    }

    static void delay(uint32_t ms) { SDL_Delay(ms); }

  private:
    const IFramebuffer& fb_;
    SDL_Window* window_{nullptr};
    SDL_Renderer* renderer_{nullptr};
    int scale_;
    bool enabled_{true};
    bool space_pressed_{false};
};

}  // namespace hub75::SDL

#endif /* HUB75_SDLDRIVER_H */
