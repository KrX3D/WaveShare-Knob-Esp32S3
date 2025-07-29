#include "rotary_encoder_custom.h"
#include "esphome/core/log.h"

namespace esphome {
namespace rotary_encoder_custom {

static const char *TAG = "rotary_encoder_custom";

void RotaryEncoderCustom::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Rotary Encoder Custom...");
  ESP_LOGI(TAG, "✨ RotaryEncoderCustom.setup() for '%s'", this->get_name().c_str());

  // Initialize pins
  this->pin_a_->setup();
  this->pin_b_->setup();

  // Read initial levels
  this->last_a_ = this->pin_a_->digital_read();
  this->last_b_ = this->pin_b_->digital_read();

  // Optionally publish the zero value on startup
  if (this->publish_initial_value_) {
    this->publish_state(this->counter_);
  }
}

void RotaryEncoderCustom::dump_config() {
  ESP_LOGCONFIG(TAG, "🕹️ Rotary Encoder Custom DUMP:");
  LOG_PIN("  Pin A: ", this->pin_a_);
  LOG_PIN("  Pin B: ", this->pin_b_);
  ESP_LOGCONFIG(TAG, "  Publish initial: %s",
                (this->publish_initial_value_ ? "YES" : "NO"));
}

void RotaryEncoderCustom::loop() {
  ESP_LOGVV(TAG, "🔄 loop()");
  this->read_encoder();
}

void RotaryEncoderCustom::read_encoder() {
  // Simple debounce: ignore if <2 ms since last change
  uint32_t now = millis();
  if (now - this->last_interrupt_time_ < 2) {
    return;
  }

  bool a = this->pin_a_->digital_read();
  bool b = this->pin_b_->digital_read();

  // Only on A-edge do we count and publish
  if (a != this->last_a_) {
    ESP_LOGD(TAG, "Pin A edge: a=%d last_a=%d b=%d", a, this->last_a_, b);
    this->last_interrupt_time_ = now;

    // Quadrature logic: if phases equal → clockwise, else counter‑clockwise
    if (a == b) {
      ESP_LOGD(TAG, "👉 Clockwise detected");
      this->counter_++;
    } else {
      ESP_LOGD(TAG, "👈 Counter-clockwise detected");
      this->counter_--;
    }
    ESP_LOGD(TAG, "Rotation, counter: %d", this->counter_);

    // Publish new value
    this->publish_state(this->counter_);
    this->last_a_ = a;
  }

  // Keep B’s last state up‑to‑date
  if (b != this->last_b_) {
    this->last_b_ = b;
  }
}

}  // namespace rotary_encoder_custom
}  // namespace esphome
