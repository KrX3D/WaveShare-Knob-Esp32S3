#include "rotary_encoder_custom.h"
#include "esphome/core/log.h"

namespace esphome {
namespace rotary_encoder_custom {

static const char *const TAG = "rotary_encoder_custom";

void RotaryEncoderCustom::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Rotary Encoder Custom...");
  
  this->pin_a_->setup();
  this->pin_b_->setup();
  
  // Read initial states
  this->last_a_ = this->pin_a_->digital_read();
  this->last_b_ = this->pin_b_->digital_read();
  
  if (this->publish_initial_value_) {
    this->publish_state(this->counter_);
  }
}

void RotaryEncoderCustom::dump_config() {
  ESP_LOGCONFIG(TAG, "Rotary Encoder Custom:");
  LOG_PIN("  Pin A: ", this->pin_a_);
  LOG_PIN("  Pin B: ", this->pin_b_);
  ESP_LOGCONFIG(TAG, "  Current value: %d", this->counter_);
}

void RotaryEncoderCustom::loop() {
  this->read_encoder();
}

void RotaryEncoderCustom::read_encoder() {
  // Simple debouncing - ignore rapid changes
  uint32_t now = millis();
  if (now - this->last_interrupt_time_ < 2) {
    return;
  }
  
  bool a = this->pin_a_->digital_read();
  bool b = this->pin_b_->digital_read();
  
  // Check if pin A changed
  if (a != this->last_a_) {
    this->last_interrupt_time_ = now;
    
    // Determine direction based on pin B state when pin A changes
    if (a == b) {
      // Clockwise rotation
      this->counter_++;
      ESP_LOGD(TAG, "Clockwise rotation, counter: %d", this->counter_);
    } else {
      // Counter-clockwise rotation
      this->counter_--;  
      ESP_LOGD(TAG, "Counter-clockwise rotation, counter: %d", this->counter_);
    }
    
    this->publish_state(this->counter_);
    this->last_a_ = a;
  }
  
  // Update last state for B as well (for more accurate readings)
  if (b != this->last_b_) {
    this->last_b_ = b;
  }
}

}  // namespace rotary_encoder_custom
}  // namespace esphome