#include "rotary_encoder_custom.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace rotary_encoder_custom {

static const char *const TAG = "rotary_encoder_custom";
static constexpr uint8_t DEBOUNCE_TICKS = 2;

void IRAM_ATTR HOT RotaryEncoderSensorStore::gpio_intr(RotaryEncoderSensorStore *arg) {
  // Just flag that we need to read in loop()
  arg->needs_update = true;
}

void RotaryEncoderCustom::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Rotary Encoder Custom...");

  int32_t initial_value = 0;
  switch (this->restore_mode_) {
    case ROTARY_ENCODER_RESTORE_DEFAULT_ZERO:
      this->rtc_ = global_preferences->make_preference<int32_t>(this->get_object_id_hash());
      if (!this->rtc_.load(&initial_value)) {
        initial_value = 0;
      }
      break;
    case ROTARY_ENCODER_ALWAYS_ZERO:
      initial_value = 0;
      break;
  }
  initial_value = clamp(initial_value, this->store_.min_value, this->store_.max_value);

  this->store_.counter = initial_value;
  this->store_.last_read = initial_value;

  this->pin_a_->setup();
  this->store_.pin_a = this->pin_a_->to_isr();
  this->pin_b_->setup();
  this->store_.pin_b = this->pin_b_->to_isr();

  if (this->pin_i_ != nullptr) {
    this->pin_i_->setup();
  }

  // Read initial pin states
  this->last_a_ = this->pin_a_->digital_read();
  this->last_b_ = this->pin_b_->digital_read();
}

void RotaryEncoderCustom::dump_config() {
  LOG_SENSOR("", "Rotary Encoder Custom", this);
  LOG_PIN("  Pin A: ", this->pin_a_);
  LOG_PIN("  Pin B: ", this->pin_b_);
  LOG_PIN("  Pin I: ", this->pin_i_);

  const LogString *restore_mode;
  switch (this->restore_mode_) {
    case ROTARY_ENCODER_RESTORE_DEFAULT_ZERO:
      restore_mode = LOG_STR("Restore (Defaults to zero)");
      break;
    case ROTARY_ENCODER_ALWAYS_ZERO:
      restore_mode = LOG_STR("Always zero");
      break;
    default:
      restore_mode = LOG_STR("");
  }
  ESP_LOGCONFIG(TAG, "  Restore Mode: %s", LOG_STR_ARG(restore_mode));
  
  if (this->store_.min_value != INT32_MIN) {
    ESP_LOGCONFIG(TAG, "  Min value: %" PRId32, this->store_.min_value);
  }
  if (this->store_.max_value != INT32_MAX) {
    ESP_LOGCONFIG(TAG, "  Max value: %" PRId32, this->store_.max_value);
  }
}

void RotaryEncoderCustom::loop() {
  // Always poll the encoder
  this->read_encoder();
  
  // Read current counter value
  int32_t counter = this->store_.counter;
  int32_t last_read = this->store_.last_read;

  // Detect direction change and fire triggers
  if (counter > last_read) {
    for (int32_t i = last_read; i < counter; i++) {
      this->on_clockwise_callback_.call();
    }
  } else if (counter < last_read) {
    for (int32_t i = last_read; i > counter; i--) {
      this->on_anticlockwise_callback_.call();
    }
  }

  // Check reset pin and publish state
  if (this->pin_i_ != nullptr && this->pin_i_->digital_read()) {
    this->store_.counter = 0;
    counter = 0;
  }

  if (this->store_.last_read != counter || this->publish_initial_value_) {
    if (this->restore_mode_ == ROTARY_ENCODER_RESTORE_DEFAULT_ZERO) {
      this->rtc_.save(&counter);
    }
    this->store_.last_read = counter;
    this->publish_state(counter);
    this->listeners_.call(counter);
    this->publish_initial_value_ = false;
  }
}

void RotaryEncoderCustom::read_encoder() {
  bool a = this->pin_a_->digital_read();
  bool b = this->pin_b_->digital_read();

  // Process A channel - CW rotation
  if (!a) {
    if (a != this->last_a_)
      this->debounce_a_cnt_ = 0;
    else if (this->debounce_a_cnt_ < 255)
      this->debounce_a_cnt_++;
  } else {
    if (a != this->last_a_ && ++this->debounce_a_cnt_ >= DEBOUNCE_TICKS) {
      this->debounce_a_cnt_ = 0;
      if (this->store_.counter < this->store_.max_value) {
        this->store_.counter++;
      }
    } else if (a == this->last_a_) {
      this->debounce_a_cnt_ = 0;
    }
  }
  this->last_a_ = a;

  // Process B channel - CCW rotation
  if (!b) {
    if (b != this->last_b_)
      this->debounce_b_cnt_ = 0;
    else if (this->debounce_b_cnt_ < 255)
      this->debounce_b_cnt_++;
  } else {
    if (b != this->last_b_ && ++this->debounce_b_cnt_ >= DEBOUNCE_TICKS) {
      this->debounce_b_cnt_ = 0;
      if (this->store_.counter > this->store_.min_value) {
        this->store_.counter--;
      }
    } else if (b == this->last_b_) {
      this->debounce_b_cnt_ = 0;
    }
  }
  this->last_b_ = b;
}

float RotaryEncoderCustom::get_setup_priority() const { return setup_priority::DATA; }

void RotaryEncoderCustom::set_restore_mode(RotaryEncoderRestoreMode restore_mode) {
  this->restore_mode_ = restore_mode;
}

void RotaryEncoderCustom::set_min_value(int32_t min_value) { this->store_.min_value = min_value; }

void RotaryEncoderCustom::set_max_value(int32_t max_value) { this->store_.max_value = max_value; }

}  // namespace rotary_encoder_custom
}  // namespace esphome
