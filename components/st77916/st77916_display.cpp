#include "st77916_display.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/core/helpers.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace st77916 {

static const char *const TAG = "st77916";

// Initialization commands for ST77916 (from your Arduino code)
static const uint8_t ST77916_INIT_SEQUENCE[] = {
    0xF0, 1, 0x28,
    0xF2, 1, 0x28,
    0x73, 1, 0xF0,
    0x7C, 1, 0xD1,
    0x83, 1, 0xE0,
    0x84, 1, 0x61,
    0xF2, 1, 0x82,
    0xF0, 1, 0x00,
    0xF0, 1, 0x01,
    0xF1, 1, 0x01,
    0xB0, 1, 0x56,
    0xB1, 1, 0x4D,
    0xB2, 1, 0x24,
    0xB4, 1, 0x87,
    0xB5, 1, 0x44,
    0xB6, 1, 0x8B,
    0xB7, 1, 0x40,
    0xB8, 1, 0x86,
    0xBA, 1, 0x00,
    0xBB, 1, 0x08,
    0xBC, 1, 0x08,
    0xBD, 1, 0x00,
    0xC0, 1, 0x80,
    0xC1, 1, 0x10,
    0xC2, 1, 0x37,
    0xC3, 1, 0x80,
    0xC4, 1, 0x10,
    0xC5, 1, 0x37,
    0xC6, 1, 0xA9,
    0xC7, 1, 0x41,
    0xC8, 1, 0x01,
    0xC9, 1, 0xA9,
    0xCA, 1, 0x41,
    0xCB, 1, 0x01,
    0xD0, 1, 0x91,
    0xD1, 1, 0x68,
    0xD2, 1, 0x68,
    0xF5, 2, 0x00, 0xA5,
    0xDD, 1, 0x4F,
    0xDE, 1, 0x4F,
    0xF1, 1, 0x10,
    0xF0, 1, 0x00,
    0xF0, 1, 0x02,
    0xE0, 14, 0xF0, 0x0A, 0x10, 0x09, 0x09, 0x36, 0x35, 0x33, 0x4A, 0x29, 0x15, 0x15, 0x2E, 0x34,
    0xE1, 14, 0xF0, 0x0A, 0x0F, 0x08, 0x08, 0x05, 0x34, 0x33, 0x4A, 0x39, 0x15, 0x15, 0x2D, 0x33,
    0xF0, 1, 0x10,
    0xF3, 1, 0x10,
    0xE0, 1, 0x07,
    0xE1, 1, 0x00,
    0xE2, 1, 0x00,
    0xE3, 1, 0x00,
    0xE4, 1, 0xE0,
    0xE5, 1, 0x06,
    0xE6, 1, 0x21,
    0xE7, 1, 0x01,
    0xE8, 1, 0x05,
    0xE9, 1, 0x02,
    0xEA, 1, 0xDA,
    0xEB, 1, 0x00,
    0xEC, 1, 0x00,
    0xED, 1, 0x0F,
    0xEE, 1, 0x00,
    0xEF, 1, 0x00,
    0xF8, 1, 0x00,
    0xF9, 1, 0x00,
    0xFA, 1, 0x00,
    0xFB, 1, 0x00,
    0xFC, 1, 0x00,
    0xFD, 1, 0x00,
    0xFE, 1, 0x00,
    0xFF, 1, 0x00,
    0x60, 1, 0x40,
    0x61, 1, 0x04,
    0x62, 1, 0x00,
    0x63, 1, 0x42,
    0x64, 1, 0xD9,
    0x65, 1, 0x00,
    0x66, 1, 0x00,
    0x67, 1, 0x00,
    0x68, 1, 0x00,
    0x69, 1, 0x00,
    0x6A, 1, 0x00,
    0x6B, 1, 0x00,
    // ... (continuing with all your init commands)
    0x11, 0, // Sleep out
    0x29, 0, // Display on
};

void ST77916Display::setup() {
  ESP_LOGD(TAG, "Setting up ST77916...");
  
  this->spi_setup();

  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
    this->reset_pin_->digital_write(true);
    delay(1);
    this->reset_pin_->digital_write(false);
    delay(10);
    this->reset_pin_->digital_write(true);
    delay(10);
  }

  if (this->backlight_pin_ != nullptr) {
    this->backlight_pin_->setup();
    this->backlight_pin_->digital_write(true);
  }

  this->init_lcd_();
  this->fill(COLOR_BLACK);
  this->display();
}

void ST77916Display::dump_config() {
  ESP_LOGCONFIG(TAG, "ST77916 Display");
  ESP_LOGCONFIG(TAG, "  Model: ST77916");
  ESP_LOGCONFIG(TAG, "  Width: %d", this->width_);
  ESP_LOGCONFIG(TAG, "  Height: %d", this->height_);
  ESP_LOGCONFIG(TAG, "  Rotation: %d", this->rotation_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  LOG_PIN("  Backlight Pin: ", this->backlight_pin_);
  LOG_UPDATE_INTERVAL(this);
}

float ST77916Display::get_setup_priority() const { return setup_priority::PROCESSOR; }

void ST77916Display::update() { this->do_update_(); }

void ST77916Display::fill(Color color) {
  uint16_t new_color = 0;
  this->x_low_ = 0;
  this->y_low_ = 0;
  this->x_high_ = this->get_width_internal() - 1;
  this->y_high_ = this->get_height_internal() - 1;
  new_color = display::ColorUtil::color_to_565(color);

  this->enable();
  this->set_addr_window_(0, 0, this->get_width_internal(), this->get_height_internal());
  this->write_command_(ST77916_RAMWR);
  
  for (uint32_t i = 0; i < (this->get_width_internal()) * (this->get_height_internal()); i++) {
    this->write_data_(new_color >> 8);
    this->write_data_(new_color & 0xFF);
  }
  this->disable();
}

void ST77916Display::display() {
  // Use the standard display buffer approach
  if (this->buffer_ == nullptr) {
    return;
  }

  this->enable();
  this->set_addr_window_(0, 0, this->get_width_internal(), this->get_height_internal());
  this->write_command_(ST77916_RAMWR);

  size_t len = this->get_buffer_length_();
  const uint8_t *buffer = this->buffer_;

  for (size_t i = 0; i < len; i += 2) {
    this->write_data_(buffer[i]);
    this->write_data_(buffer[i + 1]);
  }

  this->disable();
}

void ST77916Display::init_lcd_() {
  uint8_t cmd, x, num_args;
  const uint8_t *addr = ST77916_INIT_SEQUENCE;
  
  while (true) {
    cmd = *addr++;
    if (cmd == 0xFF) break; // End of sequence
    
    x = *addr++;
    num_args = x & 0x7F;
    
    this->write_command_(cmd);
    if (num_args > 0) {
      this->write_array_(addr, num_args);
      addr += num_args;
    }
    
    if (x & 0x80) { // If high bit set, delay follows
      delay(150);
    }
  }

  // Set color mode to 16-bit
  this->write_command_(ST77916_COLMOD);
  this->write_data_(0x55); // 16-bit color

  // Apply rotation and mirroring
  uint8_t madctl = 0;
  if (this->swap_xy_) madctl |= ST77916_MADCTL_MV;
  if (this->mirror_x_) madctl |= ST77916_MADCTL_MX;
  if (this->mirror_y_) madctl |= ST77916_MADCTL_MY;
  if (this->use_bgr_) madctl |= ST77916_MADCTL_BGR;
  
  this->write_command_(ST77916_MADCTL);
  this->write_data_(madctl);

  if (this->invert_colors_) {
    this->write_command_(ST77916_INVON);
  } else {
    this->write_command_(ST77916_INVOFF);
  }

  this->write_command_(ST77916_SLPOUT);
  delay(120);
  this->write_command_(ST77916_DISPON);
}

void ST77916Display::set_addr_window_(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  uint16_t x_start = x1 + this->offset_x_;
  uint16_t x_end = x2 + this->offset_x_;
  uint16_t y_start = y1 + this->offset_y_;
  uint16_t y_end = y2 + this->offset_y_;

  this->write_command_(ST77916_CASET);
  this->write_data_(x_start >> 8);
  this->write_data_(x_start & 0xFF);
  this->write_data_(x_end >> 8);
  this->write_data_(x_end & 0xFF);

  this->write_command_(ST77916_RASET);
  this->write_data_(y_start >> 8);
  this->write_data_(y_start & 0xFF);
  this->write_data_(y_end >> 8);
  this->write_data_(y_end & 0xFF);
}

void ST77916Display::invert_display_(bool invert) {
  this->write_command_(invert ? ST77916_INVON : ST77916_INVOFF);
}

void ST77916Display::write_command_(uint8_t value) {
  this->dc_pin_->digital_write(false);
  this->write_byte(value);
}

void ST77916Display::write_data_(uint8_t value) {
  this->dc_pin_->digital_write(true);
  this->write_byte(value);
}

void ST77916Display::write_array_(const uint8_t *data, size_t len) {
  this->dc_pin_->digital_write(true);
  this->write_array(data, len);
}

void ST77916Display::write_color_(uint16_t color, uint16_t size) {
  uint8_t hi = color >> 8;
  uint8_t lo = color & 0xFF;
  this->dc_pin_->digital_write(true);
  for (uint16_t i = 0; i < size; i++) {
    this->write_byte(hi);
    this->write_byte(lo);
  }
}

int ST77916Display::get_width_internal() { return this->width_; }
int ST77916Display::get_height_internal() { return this->height_; }

size_t ST77916Display::get_buffer_length_() { 
  return size_t(this->get_width_internal()) * size_t(this->get_height_internal()) * 2; 
}

void ST77916Display::draw_absolute_pixel_internal(int x, int y, Color color) {
  if (x >= this->get_width_internal() || x < 0 || y >= this->get_height_internal() || y < 0) {
    return;
  }

  uint32_t pos = (y * this->width_ + x) * 2;
  uint16_t new_color = display::ColorUtil::color_to_565(color);

  if (this->buffer_ != nullptr) {
    this->buffer_[pos] = new_color >> 8;
    this->buffer_[pos + 1] = new_color & 0xFF;
  }
}

}  // namespace st77916
}  // namespace esphome