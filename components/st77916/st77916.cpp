#include "st77916.h"
#include "esphome/core/log.h"

namespace esphome {
namespace st77916 {

static const char *TAG = "st77916";

void ST77916::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ST77916...");
  this->dc_pin_->setup();
  if (this->rst_pin_) this->rst_pin_->setup();
  this->spi_setup();

  // Hardware reset
  if (this->rst_pin_) {
    this->rst_pin_->digital_write(false);
    delay(10);
    this->rst_pin_->digital_write(true);
    delay(120);
  }

  this->init_lcd_();
  this->fill(COLOR_BLACK);
  this->display();
}

void ST77916::dump_config() {
  LOG_DISPLAY("", "ST77916", this);
  LOG_PIN("  DC Pin: ", this->dc_pin_);
  if (this->rst_pin_) LOG_PIN("  RST Pin: ", this->rst_pin_);
  ESP_LOGCONFIG(TAG, "  Dimensions: %ux%u", this->width_, this->height_);
}

void ST77916::update() {
  this->display();  // push buffer to screen
}

// Write one pixel into the buffer (RGB565)
void ST77916::draw_absolute_pixel(int x, int y, Color color) {
  if (x < 0 || y < 0 || x >= this->width_ || y >= this->height_) return;
  uint16_t idx = (y * this->width_ + x) * 2;
  uint16_t c = display::ColorUtil::color_to_565(color);
  this->buffer_[idx]     = c >> 8;
  this->buffer_[idx + 1] = c & 0xFF;
}

// Fill entire buffer
void ST77916::fill(Color color) {
  uint16_t c = display::ColorUtil::color_to_565(color);
  for (size_t i = 0; i < this->buffer_size_; i += 2) {
    this->buffer_[i]     = c >> 8;
    this->buffer_[i + 1] = c & 0xFF;
  }
}

// Send your init commands (same as your Arduino array)
void ST77916::init_lcd_() {
  static const uint8_t cmds[] PROGMEM = {
    // … your 0xF0,1,0x28, 0xF2,1,0x28, … 0x00
  };
  const uint8_t *ptr = cmds;
  while (true) {
    uint8_t cmd = pgm_read_byte(ptr++);
    if (!cmd) break;
    uint8_t x = pgm_read_byte(ptr++);
    uint8_t num = x & 0x7F;
    write_command_(cmd);
    for (uint8_t i = 0; i < num; i++) {
      uint8_t d = pgm_read_byte(ptr++);
      write_data_(&d, 1);
    }
    if (x & 0x80) delay(150);
  }
  delay(120);
}

void ST77916::write_command_(uint8_t cmd) {
  this->dc_pin_->digital_write(false);
  this->enable();
  this->write_byte(cmd);
  this->disable();
}
void ST77916::write_data_(const uint8_t *data, size_t len) {
  this->dc_pin_->digital_write(true);
  this->enable();
  for (size_t i = 0; i < len; i++) this->write_byte(data[i]);
  this->disable();
}

}  // namespace st77916
}  // namespace esphome
