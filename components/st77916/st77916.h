#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace st77916 {

class ST77916 : public display::DisplayBuffer, public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST,
                                                                       spi::CLOCK_POLARITY_LOW,
                                                                       spi::CLOCK_PHASE_LEADING,
                                                                       spi::DATA_RATE_40MHZ> {
 public:
  void set_dc_pin_gpio(GPIOPin *dc) { this->dc_pin_ = dc; }
  void set_reset_pin_gpio(GPIOPin *rst) { this->rst_pin_ = rst; }

  void set_dimensions(uint16_t width, uint16_t height) {
    this->width_ = width;
    this->height_ = height;
    this->init_buffer(width * height * 2);
  }

  void setup() override;
  void dump_config() override;
  void update() override;

 protected:
  // Draw one pixel into the buffer
  void draw_absolute_pixel(int x, int y, Color color) override;
  void fill(Color color) override;

  // SPI and pins
  GPIOPin *dc_pin_{nullptr};
  GPIOPin *rst_pin_{nullptr};
  uint16_t width_{360}, height_{360};

  // Your init-commands array here...
  void init_lcd_();
  void write_command_(uint8_t cmd);
  void write_data_(const uint8_t *data, size_t len);
};

}  // namespace st77916
}  // namespace esphome
