#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/automation.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/preferences.h"

namespace esphome {
namespace rotary_encoder_custom {

enum RotaryEncoderRestoreMode {
  ROTARY_ENCODER_RESTORE_DEFAULT_ZERO,
  ROTARY_ENCODER_ALWAYS_ZERO,
};

struct RotaryEncoderSensorStore {
  volatile int32_t counter{0};
  int32_t min_value{INT32_MIN};
  int32_t max_value{INT32_MAX};
  int32_t last_read{0};
};

class RotaryEncoderCustom : public sensor::Sensor, public Component {
 public:
  void set_pin_a(InternalGPIOPin *pin_a) { pin_a_ = pin_a; }
  void set_pin_b(InternalGPIOPin *pin_b) { pin_b_ = pin_b; }
  void set_restore_mode(RotaryEncoderRestoreMode restore_mode);
  void set_reset_pin(GPIOPin *pin_i) { this->pin_i_ = pin_i; }
  void set_min_value(int32_t min_value);
  void set_max_value(int32_t max_value);
  void set_publish_initial_value(bool publish_initial_value) { publish_initial_value_ = publish_initial_value; }

  void set_value(int32_t value) {
    value = clamp(value, this->store_.min_value, this->store_.max_value);
    this->store_.counter = value;
    this->store_.last_read = value;
    if (this->restore_mode_ == ROTARY_ENCODER_RESTORE_DEFAULT_ZERO) {
      this->rtc_.save(&value);
    }
    this->publish_state(value);
    this->listeners_.call(value);
  }

  void setup() override;
  void dump_config() override;
  void loop() override;
  float get_setup_priority() const override;

  void add_on_clockwise_callback(std::function<void()> callback) {
    this->on_clockwise_callback_.add(std::move(callback));
  }
  void add_on_anticlockwise_callback(std::function<void()> callback) {
    this->on_anticlockwise_callback_.add(std::move(callback));
  }
  void register_listener(std::function<void(int32_t)> listener) {
    this->listeners_.add(std::move(listener));
  }

 protected:
  InternalGPIOPin *pin_a_;
  InternalGPIOPin *pin_b_;
  GPIOPin *pin_i_{nullptr};
  bool publish_initial_value_;
  ESPPreferenceObject rtc_;
  RotaryEncoderRestoreMode restore_mode_{ROTARY_ENCODER_RESTORE_DEFAULT_ZERO};

  RotaryEncoderSensorStore store_{};

  bool last_a_{false};
  bool last_b_{false};
  uint8_t debounce_a_cnt_{0};
  uint8_t debounce_b_cnt_{0};

  void read_encoder();

  CallbackManager<void()> on_clockwise_callback_{};
  CallbackManager<void()> on_anticlockwise_callback_{};
  CallbackManager<void(int32_t)> listeners_{};
};

template<typename... Ts> class RotaryEncoderSetValueAction : public Action<Ts...> {
 public:
  RotaryEncoderSetValueAction(RotaryEncoderCustom *encoder) : encoder_(encoder) {}
  TEMPLATABLE_VALUE(int32_t, value)
  void play(Ts... x) override { this->encoder_->set_value(this->value_.value(x...)); }
 protected:
  RotaryEncoderCustom *encoder_;
};

class RotaryEncoderClockwiseTrigger : public Trigger<> {
 public:
  explicit RotaryEncoderClockwiseTrigger(RotaryEncoderCustom *parent) {
    parent->add_on_clockwise_callback([this]() { this->trigger(); });
  }
};

class RotaryEncoderAnticlockwiseTrigger : public Trigger<> {
 public:
  explicit RotaryEncoderAnticlockwiseTrigger(RotaryEncoderCustom *parent) {
    parent->add_on_anticlockwise_callback([this]() { this->trigger(); });
  }
};

}  // namespace rotary_encoder_custom
}  // namespace esphome