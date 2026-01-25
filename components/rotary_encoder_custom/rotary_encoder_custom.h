#pragma once

#include <array>

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/automation.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/preferences.h"

namespace esphome {
namespace rotary_encoder_custom {

/// All possible restore modes for the rotary encoder
enum RotaryEncoderRestoreMode {
  ROTARY_ENCODER_RESTORE_DEFAULT_ZERO,  /// try to restore counter, otherwise set to zero
  ROTARY_ENCODER_ALWAYS_ZERO,           /// do not restore counter, always set to zero
};

struct RotaryEncoderSensorStore {
  ISRInternalGPIOPin pin_a;
  ISRInternalGPIOPin pin_b;

  volatile int32_t counter{0};
  int32_t min_value{INT32_MIN};
  int32_t max_value{INT32_MAX};
  int32_t last_read{0};
  
  volatile bool needs_update{false};

  static void gpio_intr(RotaryEncoderSensorStore *arg);
};

class RotaryEncoderCustom : public sensor::Sensor, public Component {
 public:
  void set_pin_a(InternalGPIOPin *pin_a) { pin_a_ = pin_a; }
  void set_pin_b(InternalGPIOPin *pin_b) { pin_b_ = pin_b; }

  /** Set the restore mode of the rotary encoder.
   *
   * By default (if possible) the last known counter state is restored. Otherwise the value 0 is used.
   * Restoring the state can also be turned off.
   *
   * @param restore_mode The restore mode to use.
   */
  void set_restore_mode(RotaryEncoderRestoreMode restore_mode);

  /// Manually set the value of the counter.
  void set_value(int value) {
    this->store_.counter = value;
    this->loop();
  }

  void set_reset_pin(GPIOPin *pin_i) { this->pin_i_ = pin_i; }
  void set_min_value(int32_t min_value);
  void set_max_value(int32_t max_value);
  void set_publish_initial_value(bool publish_initial_value) { publish_initial_value_ = publish_initial_value; }

  // ========== INTERNAL METHODS ==========
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

  void register_listener(std::function<void(int32_t)> listener) { this->listeners_.add(std::move(listener)); }

 protected:
  InternalGPIOPin *pin_a_;
  InternalGPIOPin *pin_b_;
  GPIOPin *pin_i_{nullptr};  /// Index pin, if this is not nullptr, the counter will reset to 0 once this pin is HIGH.
  bool publish_initial_value_;
  ESPPreferenceObject rtc_;
  RotaryEncoderRestoreMode restore_mode_{ROTARY_ENCODER_RESTORE_DEFAULT_ZERO};

  RotaryEncoderSensorStore store_{};
  
  // Polling state
  bool last_a_{false};
  bool last_b_{false};
  uint8_t debounce_a_cnt_{0};
  uint8_t debounce_b_cnt_{0};
  
  void read_encoder();

  CallbackManager<void()> on_clockwise_callback_{};
  CallbackManager<void()> on_anticlockwise_callback_{};
  CallbackManager<void(int32_t)> listeners_{};
};

// Action to set a specific value
template<typename... Ts> class RotaryEncoderSetValueAction : public Action<Ts...> {
 public:
  RotaryEncoderSetValueAction(RotaryEncoderCustom *encoder) : encoder_(encoder) {}
  TEMPLATABLE_VALUE(int, value)

  void play(Ts... x) override { this->encoder_->set_value(this->value_.value(x...)); }

 protected:
  RotaryEncoderCustom *encoder_;
};

// Trigger for clockwise rotation
class RotaryEncoderClockwiseTrigger : public Trigger<> {
 public:
  explicit RotaryEncoderClockwiseTrigger(RotaryEncoderCustom *parent) {
    parent->add_on_clockwise_callback([this]() { this->trigger(); });
  }
};

// Trigger for anticlockwise rotation
class RotaryEncoderAnticlockwiseTrigger : public Trigger<> {
 public:
  explicit RotaryEncoderAnticlockwiseTrigger(RotaryEncoderCustom *parent) {
    parent->add_on_anticlockwise_callback([this]() { this->trigger(); });
  }
};

}  // namespace rotary_encoder_custom
}  // namespace esphome
