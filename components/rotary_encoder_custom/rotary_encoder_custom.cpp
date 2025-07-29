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
  if (now - last_interrupt_time_ < 2) return;

  bool a = pin_a_->digital_read();
  bool b = pin_b_->digital_read();

  if (a != last_a_) {
    ESP_LOGD(TAG, "Pin A edge: a=%d last_a=%d b=%d", a, last_a_, b);
    last_interrupt_time_ = now;

    if (a == b) {
      ESP_LOGD(TAG, "👉 Clockwise");
      counter_++;
    } else {
      ESP_LOGD(TAG, "👈 Counter‑clockwise");
      counter_--;
    }
    ESP_LOGD(TAG, "Counter: %d", counter_);
    publish_state(counter_);
    last_a_ = a;
  }

  if (b != last_b_) {
    last_b_ = b;
  }
}

}  // namespace rotary_encoder_custom
}  // namespace esphome
