import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID

DEPENDENCIES = []

rotary_encoder_custom_ns = cg.esphome_ns.namespace("rotary_encoder_custom")
RotaryEncoderCustom = rotary_encoder_custom_ns.class_(
    "RotaryEncoderCustom", cg.Component, sensor.Sensor
)

CONFIG_SCHEMA = cv.Schema({})

async def to_code(config):
    pass