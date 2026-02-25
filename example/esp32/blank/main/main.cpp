#include "Framebuffer.hpp"
#include "esp/ESPDriver.hpp"

extern "C" void app_main() {
    static hub75::Framebuffer<64, 64> fb;
    hub75::esp::ESPDriver<64, 64, hub75::ScanRate::Scan1_32> driver(fb);

    fb.fill(hub75::Color::red());

    while (true) {
        driver.refresh();
    }
}
