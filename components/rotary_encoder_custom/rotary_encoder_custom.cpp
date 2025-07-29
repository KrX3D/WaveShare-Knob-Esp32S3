#include "rotary_encoder_custom.h"
#include "esphome/core/log.h"

namespace esphome {
namespace rotary_encoder_custom {

static const char *TAG = "rotary_encoder_custom";

void RotaryEncoderCustom::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Rotary Encoder Custom...");
  ESP_LOGI(TAG, "✨ setup() for '%s'", this->get_name().c_str());

  pin_a_->setup();
  pin_b_->setup();
  last_a_ = pin_a_->digital_read();
  last_b_ = pin_b_->digital_read();

  if (publish_initial_value_) {
    this->publish_state(counter_);
  }
}

void RotaryEncoderCustom::dump_config() {
  ESP_LOGCONFIG(TAG, "🕹️ Rotary Encoder Custom DUMP:");
  LOG_PIN("  Pin A: ", pin_a_);
  LOG_PIN("  Pin B: ", pin_b_);
  ESP_LOGCONFIG(TAG, "  Publish initial: %s", publish_initial_value_ ? "YES" : "NO");
}

void RotaryEncoderCustom::loop() {
  ESP_LOGVV(TAG, "🔄 loop()");
  read_encoder();
}

void RotaryEncoderCustom::read_encoder() {
  uint32_t now = millis();
  // Debounce: skip reads faster than 2ms
  if (now - last_interrupt_time_ < 2) return;

  bool a = pin_a_->digital_read();
  bool b = pin_b_->digital_read();
  uint8_t state = (a << 1) | b;      // new two‑bit state
  uint8_t prev  = last_state_;       // old two‑bit state

  if (state != prev) {
    // update time
    last_interrupt_time_ = now;

    // Calculate direction:
    // diff = (prev - state) mod 4
    uint8_t diff = (prev - state) & 0x03;
    if (diff == 1) {
      // 00→01→11→10→00 clockwise
      counter_++;
      ESP_LOGD(TAG, "👉 CW step, counter=%d", counter_);
    } else if (diff == 3) {
      // 00→10→11→01→00 counter‑clockwise
      counter_--;
      ESP_LOGD(TAG, "👈 CCW step, counter=%d", counter_);
    } else {
      // diff==2 is an invalid bounce (skip), diff==0 no change
      ESP_LOGW(TAG, "Ignored illegal step: prev=%02b new=%02b", prev, state);
    }

    // Publish only if we got a valid move
    if (diff == 1 || diff == 3) {
      this->publish_state(this->counter_);
    }

    // Save for next time
    last_state_ = state;
  }
}


}  // namespace rotary_encoder_custom
}  // namespace esphome
