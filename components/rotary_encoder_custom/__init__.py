import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import sensor
from esphome.const import CONF_ID
from esphome import pins

# Pull in the schema from sensor.py
from .sensor import CONFIG_SCHEMA as ROTARY_SENSOR_SCHEMA

CONFIG_SCHEMA = ROTARY_SENSOR_SCHEMA.extend(cv.COMPONENT_SCHEMA)

# C++ class binding
rotary_encoder_custom_ns = cg.esphome_ns.namespace("rotary_encoder_custom")
RotaryEncoderCustom = rotary_encoder_custom_ns.class_(
    "RotaryEncoderCustom", cg.Component, sensor.Sensor
)

async def to_code(config):
    # Create and register component + sensor
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)

    # Wire pins
    pin_a = await cg.gpio_pin_expression(config["pin_a"])
    cg.add(var.set_pin_a(pin_a))
    pin_b = await cg.gpio_pin_expression(config["pin_b"])
    cg.add(var.set_pin_b(pin_b))

    # Publish initial?
    cg.add(var.set_publish_initial_value(config.get("publish_initial_value", False)))
