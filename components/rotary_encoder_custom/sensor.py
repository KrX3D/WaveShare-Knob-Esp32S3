import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_EMPTY,
    STATE_CLASS_TOTAL_INCREASING,
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
        state_class=STATE_CLASS_TOTAL_INCREASING,
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


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)

    pin_a = await cg.gpio_pin_expression(config[CONF_PIN_A])
    cg.add(var.set_pin_a(pin_a))
    pin_b = await cg.gpio_pin_expression(config[CONF_PIN_B])
    cg.add(var.set_pin_b(pin_b))
    
    cg.add(var.set_publish_initial_value(config[CONF_PUBLISH_INITIAL_VALUE]))