# components/rotary_encoder_custom/sensor.py
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
  CONF_ID,
  DEVICE_CLASS_EMPTY,
  STATE_CLASS_MEASUREMENT,   # <— make sure you switched this
  UNIT_EMPTY,
)
from esphome import pins

from . import rotary_encoder_custom_ns, RotaryEncoderCustom

CONF_PIN_A = "pin_a"
CONF_PIN_B = "pin_b"
CONF_PUBLISH_INITIAL_VALUE = "publish_initial_value"

CONFIG_SCHEMA = (
  sensor.sensor_schema(
    RotaryEncoderCustom,
    unit_of_measurement=UNIT_EMPTY,
    accuracy_decimals=0,
    device_class=DEVICE_CLASS_EMPTY,
    state_class=STATE_CLASS_MEASUREMENT,
  )
  .extend(
    {
      cv.Required(CONF_PIN_A): pins.gpio_input_pin_schema,
      cv.Required(CONF_PIN_B): pins.gpio_input_pin_schema,
      cv.Optional(CONF_PUBLISH_INITIAL_VALUE, default=False): cv.boolean,
    }
  )
  .extend(cv.COMPONENT_SCHEMA)
)
