#include "Framebuffer.hpp"
#include "esp/ESPDriver.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static hub75::DoubleFramebuffer<64, 64> fb;
static hub75::esp::ESPDriver<64, 64, hub75::ScanRate::Scan1_32>* g_driver;

void display_task(void*) {
    while (true) {
        g_driver->refresh();
    }
}

void render_task(void*) {
    uint8_t row = 0;
    const auto white = hub75::Color::from_rgb888(255, 255, 255);
    const auto black = hub75::Color::from_rgb888(0, 0, 0);

    while (true) {
        auto& back = fb.back();
        back.fill(black);
        for (int x = 0; x < 64; x++) {
            back.set(static_cast<uint8_t>(x), row, white);
        }
        fb.swap();
        row = (row + 1) % 64;
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}

extern "C" void app_main() {
    static hub75::esp::ESPDriver<64, 64, hub75::ScanRate::Scan1_32> driver(fb);
    g_driver = &driver;

    xTaskCreatePinnedToCore(display_task, "display", 4096, nullptr, 5, nullptr, 1);
    xTaskCreatePinnedToCore(render_task, "render", 4096, nullptr, 4, nullptr, 0);
}
