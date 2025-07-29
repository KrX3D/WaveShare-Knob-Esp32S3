#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"                  // for esphome::hal::GPIOPin
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace rotary_encoder_custom {

// Increase if you still see missed steps at high speed
static constexpr uint8_t DEBOUNCE_TICKS = 5;

class RotaryEncoderCustom : public Component, public sensor::Sensor {
 public:
  // Bind the two GPIO pins
  void set_pin_a(hal::GPIOPin *pin_a) { this->pin_a_ = pin_a; }
  void set_pin_b(hal::GPIOPin *pin_b) { this->pin_b_ = pin_b; }

  // Optionally publish a zero value on startup
  void set_publish_initial_value(bool v) { this->publish_initial_value_ = v; }

  void setup() override;
  void dump_config() override;
  void loop() override;

 protected:
  hal::GPIOPin *pin_a_;
  hal::GPIOPin *pin_b_;
  bool publish_initial_value_{false};

  int32_t counter_{0};

  // Debounce state for each phase
  bool last_a_{false};
  bool last_b_{false};
  uint8_t debounce_a_cnt_{0};
  uint8_t debounce_b_cnt_{0};

  // Process one channel: if `clockwise==true`, HIGH after debounce → +1; else −1
  void process_channel(bool current, bool &prev,
                       uint8_t &debounce_cnt, bool clockwise);

  // Read both pins and feed into process_channel()
  void read_encoder();
};

}  // namespace rotary_encoder_custom
}  // namespace esphome
