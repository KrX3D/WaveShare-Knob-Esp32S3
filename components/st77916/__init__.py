import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display, spi

DEPENDENCIES = ["spi"]

st77916_ns = cg.esphome_ns.namespace("st77916")
ST77916 = st77916_ns.class_(
    "ST77916", 
    cg.PollingComponent, 
    display.DisplayBuffer, 
    spi.SPIDevice
)

CONFIG_SCHEMA = cv.Schema({})

async def to_code(config):
    pass