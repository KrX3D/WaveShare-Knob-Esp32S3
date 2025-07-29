# components/rotary_encoder_custom/__init__.py

import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import sensor
from esphome.const import CONF_ID
from esphome import pins

from .rotary_encoder_custom import RotaryEncoderCustom

# 1) Grab the schema from sensor.py
from .sensor import CONFIG_SCHEMA as SENSOR_SCHEMA
CONFIG_SCHEMA = SENSOR_SCHEMA.extend(cv.COMPONENT_SCHEMA)

# 2) Expose your C++ class in the esphome namespace
rotary_encoder_custom_ns = cg.esphome_ns.namespace("rotary_encoder_custom")
RotaryEncoderCustom = rotary_encoder_custom_ns.class_(
    "RotaryEncoderCustom", cg.Component, sensor.Sensor
)

async def to_code(config):
    # Create the C++ object
    var = cg.new_Pvariable(config[CONF_ID])
    # Register it as both a component and a sensor
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)

    # Wire up the two pins
    pin_a = await cg.gpio_pin_expression(config["pin_a"])
    cg.add(var.set_pin_a(pin_a))
    pin_b = await cg.gpio_pin_expression(config["pin_b"])
    cg.add(var.set_pin_b(pin_b))

    # Respect publish_initial_value
    cg.add(var.set_publish_initial_value(config.get("publish_initial_value", False)))
