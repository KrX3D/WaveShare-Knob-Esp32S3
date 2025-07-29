#include "rotary_encoder_custom.h"
#include "esphome/core/log.h"

namespace esphome {
namespace rotary_encoder_custom {

static const char *TAG = "rotary_encoder_custom";

void RotaryEncoderCustom::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Rotary Encoder Custom...");
  ESP_LOGI(TAG, "✨ setup() for '%s'", this->get_name().c_str());

  // Initialize the two GPIO pins
  pin_a_->setup();
  pin_b_->setup();

  // Capture their starting levels
  last_a_ = pin_a_->digital_read();
  last_b_ = pin_b_->digital_read();

  // Send an initial zero if requested
  if (publish_initial_value_) {
    this->publish_state(counter_);
  }
}

void RotaryEncoderCustom::dump_config() {
  ESP_LOGCONFIG(TAG, "🕹️ Rotary Encoder Custom DUMP:");
  LOG_PIN("  Pin A: ", pin_a_);
  LOG_PIN("  Pin B: ", pin_b_);
  ESP_LOGCONFIG(TAG, "  Publish initial: %s",
                publish_initial_value_ ? "YES" : "NO");
}

void RotaryEncoderCustom::loop() {
  // Always sample both pins each loop iteration
  this->read_encoder();
}

void RotaryEncoderCustom::process_channel(bool current, bool &prev,
                                          uint8_t &debounce_cnt,
                                          bool clockwise) {
  if (!current) {
    // While line is LOW, count stable readings
    if (current != prev)
      debounce_cnt = 0;
    else
      debounce_cnt++;
  } else {
    // On rising edge after debounce threshold, fire a step
    if (current != prev && ++debounce_cnt >= DEBOUNCE_TICKS) {
      debounce_cnt = 0;
      counter_ += clockwise ? 1 : -1;
      ESP_LOGD(TAG, "%s step, counter=%d",
               clockwise ? "👉 CW" : "👈 CCW", counter_);
      this->publish_state(counter_);
    } else {
      debounce_cnt = 0;
    }
  }
  prev = current;
}

void RotaryEncoderCustom::read_encoder() {
  // Read both phases and process them
  bool a = pin_a_->digital_read();
  bool b = pin_b_->digital_read();

  // A‑phase rising = CW
  process_channel(a, this->last_a_, this->debounce_a_cnt_, true);
  // B‑phase rising = CCW
  process_channel(b, this->last_b_, this->debounce_b_cnt_, false);
}

}  // namespace rotary_encoder_custom
}  // namespace esphome
