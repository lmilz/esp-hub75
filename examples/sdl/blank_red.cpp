#include "Framebuffer.hpp"
#include "sdl/SDLDriver.hpp"

int main() {
    hub75::Framebuffer<64, 64> fb;
    hub75::SDL::SDLDriver<64, 64> driver(fb);

    fb.fill(hub75::Color::red());

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }
        driver.refresh();
        SDL_Delay(16);
    }
}
