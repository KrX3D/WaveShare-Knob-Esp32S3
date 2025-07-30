#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace st77916 {

static const char *const TAG = "st77916";

enum ST77916Model {
  ST77916_MODEL = 0,
};

enum ST77916ColorMode {
  BITS_8 = 0x08,
  BITS_16 = 0x10,
};

static const uint8_t ST77916_MADCTL_MY = 0x80;   ///< Page addr order: Bottom to top
static const uint8_t ST77916_MADCTL_MX = 0x40;   ///< Column addr order: Right to left
static const uint8_t ST77916_MADCTL_MV = 0x20;   ///< Page/Column order: Reverse Mode ( X <-> Y )
static const uint8_t ST77916_MADCTL_ML = 0x10;   ///< LCD refresh Bottom to top
static const uint8_t ST77916_MADCTL_BGR = 0x08;  ///< Blue-Green-Red pixel order
static const uint8_t ST77916_MADCTL_MH = 0x04;   ///< LCD refresh right to left
static const uint8_t ST77916_MADCTL_RGB = 0x00;  ///< Red-Green-Blue pixel order

// ST77916 commands
static const uint8_t ST77916_NOP = 0x00;       // No Operation
static const uint8_t ST77916_SWRESET = 0x01;   // Software reset
static const uint8_t ST77916_SLPIN = 0x10;     // Sleep in
static const uint8_t ST77916_SLPOUT = 0x11;    // Sleep out
static const uint8_t ST77916_INVOFF = 0x20;    // Display inversion off
static const uint8_t ST77916_INVON = 0x21;     // Display inversion on
static const uint8_t ST77916_DISPOFF = 0x28;   // Display off
static const uint8_t ST77916_DISPON = 0x29;    // Display on
static const uint8_t ST77916_CASET = 0x2A;     // Column address set
static const uint8_t ST77916_RASET = 0x2B;     // Row address set
static const uint8_t ST77916_RAMWR = 0x2C;     // Memory write
static const uint8_t ST77916_MADCTL = 0x36;    // Memory data access control
static const uint8_t ST77916_COLMOD = 0x3A;    // Interface pixel format

class ST77916Display : public PollingComponent,
                       public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
                                             spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_40MHZ>,
                       public display::DisplayBuffer {
 public:
  void set_model(ST77916Model model) { this->model_ = model; }
  void set_reset_pin(GPIOPin *reset_pin) { this->reset_pin_ = reset_pin; }
  void set_backlight_pin(GPIOPin *backlight_pin) { this->backlight_pin_ = backlight_pin; }
  void set_rotation(uint8_t rotation) { this->rotation_ = rotation; }
  void set_mirror_x(bool mirror_x) { this->mirror_x_ = mirror_x; }
  void set_mirror_y(bool mirror_y) { this->mirror_y_ = mirror_y; }
  void set_swap_xy(bool swap_xy) { this->swap_xy_ = swap_xy; }
  void set_offset_x(int16_t offset_x) { this->offset_x_ = offset_x; }
  void set_offset_y(int16_t offset_y) { this->offset_y_ = offset_y; }
  void set_color_palette(display::ColorPalette color_palette) { this->color_palette_ = color_palette; }

  // ========== INTERNAL METHODS ==========
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;
  void loop() override {}

  void display();

  void fill(Color color) override;

  void write_display_data();

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  void init_lcd_();
  void set_addr_window_(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
  void invert_display_(bool invert);
  void reset_();
  void write_command_(uint8_t value);
  void write_data_(uint8_t value);
  void write_array_(const uint8_t *data, size_t len);
  void write_color_(uint16_t color, uint16_t size);

  int get_width_internal() override;
  int get_height_internal() override;

  size_t get_buffer_length_();

  ST77916Model model_{ST77916_MODEL};
  int16_t width_{360}, height_{360};
  int16_t offset_x_{0}, offset_y_{0};
  uint8_t rotation_{0};
  bool mirror_x_{false}, mirror_y_{false}, swap_xy_{false};
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *backlight_pin_{nullptr};
  display::ColorPalette color_palette_{display::ColorPalette::COLOR_PALETTE_NONE};
  bool is_18bitdisplay_{false};
  bool use_bgr_{false};
  bool invert_colors_{false};
  uint8_t color_mode_{BITS_16};
  uint32_t buffer_color_mode_{};
};

}  // namespace st77916
}  // namespace esphome