#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace rotary_encoder_custom {

class RotaryEncoderCustom : public Component, public sensor::Sensor {
 public:
  void set_pin_a(GPIOPin *pin_a) { this->pin_a_ = pin_a; }
  void set_pin_b(GPIOPin *pin_b) { this->pin_b_ = pin_b; }
  void set_publish_initial_value(bool v) { this->publish_initial_value_ = v; }

  void setup() override;
  void dump_config() override;
  void loop() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  GPIOPin *pin_a_;
  GPIOPin *pin_b_;
  bool publish_initial_value_{false};

  int32_t counter_{0};
  bool last_a_{false};
  bool last_b_{false};
  uint32_t last_interrupt_time_{0};

  void read_encoder();
};

}  // namespace rotary_encoder_custom
}  // namespace esphome
