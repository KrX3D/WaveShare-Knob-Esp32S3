#include "st77916.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace st77916 {

static const char *const TAG = "st77916";

// ST77916 initialization commands
static const uint8_t PROGMEM init_commands[] = {
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
    0x21, 1, 0x00,  // Display inversion on
    0x11, 0,        // Sleep out
    0x29, 0,        // Display on
    0x00            // End of commands
};

void ST77916::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ST77916...");
  
  this->spi_setup();
  
  this->dc_pin_ptr_ = new GPIOPin(this->dc_pin_, OUTPUT);
  this->dc_pin_ptr_->setup();
  
  if (this->reset_pin_ != 255) {
    this->reset_pin_ptr_ = new GPIOPin(this->reset_pin_, OUTPUT);
    this->reset_pin_ptr_->setup();
  }

  this->init_internal_(this->get_buffer_length_());
  this->init_lcd();
  this->fill(COLOR_BLACK);
  this->display();
}

void ST77916::dump_config() {
  LOG_DISPLAY("", "ST77916", this);
  LOG_PIN("  CS Pin: ", this->cs_);
  ESP_LOGCONFIG(TAG, "  DC Pin: %u", this->dc_pin_);
  ESP_LOGCONFIG(TAG, "  Reset Pin: %u", this->reset_pin_);
  ESP_LOGCONFIG(TAG, "  Width: %u", this->width_);
  ESP_LOGCONFIG(TAG, "  Height: %u", this->height_);
  LOG_UPDATE_INTERVAL(this);
}

float ST77916::get_setup_priority() const { return setup_priority::PROCESSOR; }

void ST77916::update() { this->do_update_(); }

void ST77916::init_lcd() {
  // Hardware reset
  if (this->reset_pin_ptr_ != nullptr) {
    this->reset_pin_ptr_->digital_write(false);
    delay(10);
    this->reset_pin_ptr_->digital_write(true);
    delay(120);
  }

  // Send initialization commands
  uint8_t cmd, x, numArgs;
  const uint8_t *addr = init_commands;
  
  while ((cmd = *addr++) > 0) {
    x = *addr++;
    numArgs = x & 0x7F;
    this->write_command(cmd);
    
    if (numArgs > 0) {
      for (uint8_t i = 0; i < numArgs; i++) {
        this->write_data(*addr++);
      }
    }
    
    if (x & 0x80) {
      delay(150);
    }
  }
  
  delay(120);
}

void ST77916::write_command(uint8_t cmd) {
  this->dc_pin_ptr_->digital_write(false);
  this->enable();
  this->write_byte(cmd);
  this->disable();
}

void ST77916::write_data(uint8_t data) {
  this->dc_pin_ptr_->digital_write(true);
  this->enable();
  this->write_byte(data);
  this->disable();
}

void ST77916::write_data16(uint16_t data) {
  this->dc_pin_ptr_->digital_write(true);
  this->enable();
  this->write_byte(data >> 8);
  this->write_byte(data & 0xFF);
  this->disable();
}

void ST77916::write_array_impl(const uint8_t *data, size_t len) {
  this->dc_pin_ptr_->digital_write(true);
  this->enable();
  this->write_array(data, len);
  this->disable();
}

void ST77916::set_addr_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  this->write_command(0x2A);  // Column address set
  this->write_data16(x1);
  this->write_data16(x2);

  this->write_command(0x2B);  // Row address set
  this->write_data16(y1);
  this->write_data16(y2);

  this->write_command(0x2C);  // Memory write
}

void ST77916::display() {
  this->set_addr_window(0, 0, this->width_ - 1, this->height_ - 1);
  this->write_display_data();
}

void ST77916::write_display_data() {
  this->dc_pin_ptr_->digital_write(true);
  this->enable();
  
  for (uint32_t i = 0; i < this->get_buffer_length_(); i += 2) {
    uint16_t pixel = (this->buffer_[i] << 8) | this->buffer_[i + 1];
    this->write_byte(pixel >> 8);
    this->write_byte(pixel & 0xFF);
  }
  
  this->disable();
}

void ST77916::draw_absolute_pixel_internal(int x, int y, Color color) {
  if (x >= this->get_width_internal() || y >= this->get_height_internal() || x < 0 || y < 0)
    return;

  uint32_t pos = (x + y * this->get_width_internal()) * 2;
  uint16_t color565 = display::ColorUtil::color_to_565(color);
  
  this->buffer_[pos] = color565 >> 8;
  this->buffer_[pos + 1] = color565 & 0xFF;
}

void ST77916::fill_internal(Color color) {
  uint16_t color565 = display::ColorUtil::color_to_565(color);
  for (uint32_t i = 0; i < this->get_buffer_length_(); i += 2) {
    this->buffer_[i] = color565 >> 8;
    this->buffer_[i + 1] = color565 & 0xFF;
  }
}

}  // namespace st77916
}  // namespace esphome