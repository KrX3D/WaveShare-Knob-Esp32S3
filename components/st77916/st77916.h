#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace st77916 {

class ST77916 : public PollingComponent, 
                public display::DisplayBuffer, 
                public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW, spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_40MHZ> {
 public:
  void set_dc_pin(uint8_t dc_pin) { dc_pin_ = dc_pin; }
  void set_reset_pin(uint8_t reset_pin) { reset_pin_ = reset_pin; }
  void set_width(uint16_t width) { this->width_ = width; }
  void set_height(uint16_t height) { this->height_ = height; }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;

  void display();

  display::DisplayType get_display_type() override { return display::DISPLAY_TYPE_COLOR; }

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  void fill_internal(Color color) override;

  void init_lcd();
  void set_addr_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
  void write_display_data();

  void write_command(uint8_t cmd);
  void write_data(uint8_t data);
  void write_data16(uint16_t data);
  void write_array_impl(const uint8_t *data, size_t len);

  uint8_t dc_pin_;
  uint8_t reset_pin_{255};
  uint16_t width_{360};
  uint16_t height_{360};

  GPIOPin *dc_pin_ptr_;
  GPIOPin *reset_pin_ptr_{nullptr};
};

}  // namespace st77916
}  // namespace esphome