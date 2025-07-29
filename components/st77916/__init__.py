import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display, spi
from esphome.const import CONF_ID, CONF_DC_PIN, CONF_RESET_PIN, CONF_WIDTH, CONF_HEIGHT

st77916_ns = cg.esphome_ns.namespace("st77916")
ST77916 = st77916_ns.class_("ST77916", display.DisplayBuffer, spi.SPIDevice)

CONFIG_SCHEMA = (
    display.FULL_DISPLAY_SCHEMA.extend({
        cv.GenerateID(): cv.declare_id(ST77916),
        cv.Required(CONF_DC_PIN): cv.gpio_pin_schema,
        cv.Optional(CONF_RESET_PIN): cv.gpio_pin_schema,
        cv.Optional(CONF_WIDTH, default=360): cv.uint16_t,
        cv.Optional(CONF_HEIGHT, default=360): cv.uint16_t,
    })
    .extend(cv.polling_component_schema("1s"))
    .extend(spi.spi_device_schema(cs_pin_required=True))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    # Bind pins & dimensions
    pin_dc = await cg.gpio_pin_expression(config[CONF_DC_PIN])
    cg.add(var.set_dc_pin_gpio(pin_dc))
    if CONF_RESET_PIN in config:
        pin_rst = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
        cg.add(var.set_reset_pin_gpio(pin_rst))
    cg.add(var.set_dimensions(config[CONF_WIDTH], config[CONF_HEIGHT]))

    await spi.register_spi_device(var, config)
    await display.register_display(var, config)
