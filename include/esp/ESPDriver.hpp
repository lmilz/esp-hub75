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
 * @file: ESPDriver.hpp
 * @brief ESP Driver
 *
 */

#ifndef HUB75_ESPDRIVER_H
#define HUB75_ESPDRIVER_H

#include "Framebuffer.hpp"
#include <driver/gpio.h>
#include <esp_rom_sys.h>

namespace hub75::esp {
template <uint8_t Width, uint8_t Height, ScanRate Scan = ScanRate::Scan1_16>
class ESPDriver {
public:
  using Config = PanelConfig<Width, Height, Scan>;

  explicit ESPDriver(IFramebuffer &fb, Config config = {})
      : config_(config), fb_(fb), enabled_(true) {
    init_gpio();
  }

  ~ESPDriver() {
    gpio_set_level(static_cast<gpio_num_t>(config_.pins.oe),
                   0); // Disable display
  }

  void set_enabled(bool enabled) {
    enabled_ = enabled;
    if (!enabled)
      gpio_set_level(static_cast<gpio_num_t>(config_.pins.oe), 1);
  }

  void refresh() {
    const auto &pins = config_.pins;

    for (uint8_t row = 0; row < Config::scan_rows; ++row) {
      gpio_set_level(static_cast<gpio_num_t>(pins.oe), 1);
      set_row_address(row, Config::addr_bits);

      // Determine framebuffer row indices
      // Upper half: row 0 to (height/2 - 1)
      // Lower half: row (height/2) to (height -1)
      const Color *row_upper = &fb_.data()[row * Config::width];
      const Color *row_lower =
          &fb_.data()[(row + Config::scan_rows) * Config::width];

      // Shift out all columns
      for (uint8_t col = 0; col < Config::width; ++col) {
        // Color mode
        gpio_set_level(static_cast<gpio_num_t>(pins.r1),
                       row_upper[col].r() != 0);
        gpio_set_level(static_cast<gpio_num_t>(pins.g1),
                       row_upper[col].g() != 0);
        gpio_set_level(static_cast<gpio_num_t>(pins.b1),
                       row_upper[col].b() != 0);

        gpio_set_level(static_cast<gpio_num_t>(pins.r2),
                       row_lower[col].r() != 0);
        gpio_set_level(static_cast<gpio_num_t>(pins.g2),
                       row_lower[col].g() != 0);
        gpio_set_level(static_cast<gpio_num_t>(pins.b2),
                       row_lower[col].b() != 0);

        // Clock pulse
        gpio_set_level(static_cast<gpio_num_t>(pins.clk), 1);
        gpio_set_level(static_cast<gpio_num_t>(pins.clk), 0);
      }

      // Latch and enable
      gpio_set_level(static_cast<gpio_num_t>(pins.lat), 1);
      gpio_set_level(static_cast<gpio_num_t>(pins.lat), 0);

      if (enabled_) {
        gpio_set_level(static_cast<gpio_num_t>(pins.oe), 0);
        esp_rom_delay_us(config_.row_time_us);
      }
    }
  }

private:
  void init_gpio() {
    const auto &pins = config_.pins;

    uint64_t pin_mask =
        (1ULL << pins.r1) | (1ULL << pins.g1) | (1ULL << pins.b1) |
        (1ULL << pins.r2) | (1ULL << pins.g2) | (1ULL << pins.b2) |
        (1ULL << pins.clk) | (1ULL << pins.lat) | (1ULL << pins.oe) |
        (1ULL << pins.addr_a) | (1ULL << pins.addr_b);

    if (Config::addr_bits >= 3 && pins.addr_c != PinConfig::UNUSED)
      pin_mask |= (1ULL << pins.addr_c);
    if (Config::addr_bits >= 4 && pins.addr_d != PinConfig::UNUSED)
      pin_mask |= (1ULL << pins.addr_d);
    if (Config::addr_bits >= 5 && pins.addr_e != PinConfig::UNUSED)
      pin_mask |= (1ULL << pins.addr_e);

    gpio_config_t io_conf = {.pin_bit_mask = pin_mask,
                             .mode = GPIO_MODE_OUTPUT,
                             .pull_up_en = GPIO_PULLUP_DISABLE,
                             .pull_down_en = GPIO_PULLDOWN_DISABLE,
                             .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io_conf);

    gpio_set_level(static_cast<gpio_num_t>(pins.oe), 1);
    gpio_set_level(static_cast<gpio_num_t>(pins.lat), 0);
    gpio_set_level(static_cast<gpio_num_t>(pins.clk), 0);

    // Clear data lines
    gpio_set_level(static_cast<gpio_num_t>(pins.r1), 0);
    gpio_set_level(static_cast<gpio_num_t>(pins.g1), 0);
    gpio_set_level(static_cast<gpio_num_t>(pins.b1), 0);
    gpio_set_level(static_cast<gpio_num_t>(pins.r2), 0);
    gpio_set_level(static_cast<gpio_num_t>(pins.g2), 0);
    gpio_set_level(static_cast<gpio_num_t>(pins.b2), 0);

    set_row_address(0, Config::addr_bits);
  }

  void set_row_address(uint8_t row, uint8_t addr_bits) {
    const auto &pins = config_.pins;

    gpio_set_level(static_cast<gpio_num_t>(pins.addr_a), row & 0x01);
    gpio_set_level(static_cast<gpio_num_t>(pins.addr_b), (row >> 1) & 0x01);

    if (addr_bits >= 3 && pins.addr_c != PinConfig::UNUSED)
      gpio_set_level(static_cast<gpio_num_t>(pins.addr_c), (row >> 2) & 0x01);
    if (addr_bits >= 4 && pins.addr_d != PinConfig::UNUSED)
      gpio_set_level(static_cast<gpio_num_t>(pins.addr_d), (row >> 3) & 0x01);
    if (addr_bits >= 5 && pins.addr_e != PinConfig::UNUSED)
      gpio_set_level(static_cast<gpio_num_t>(pins.addr_e), (row >> 4) & 0x01);
  }

  Config config_;
  IFramebuffer &fb_;
  bool enabled_;
};

} // namespace hub75::esp

#endif /* HUB75_ESPDRIVER_H */
