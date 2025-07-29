# components/st77916/__init__.py

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display, spi
from esphome.const import CONF_ID, CONF_WIDTH, CONF_HEIGHT, CONF_LAMBDA
from esphome.pins import gpio_output_pin_schema

# Bring in your C++ class
st77916_ns = cg.esphome_ns.namespace("st77916")
ST77916 = st77916_ns.class_("ST77916", display.DisplayBuffer, spi.SPIDevice)

CONFIG_SCHEMA = (
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(ST77916),
            # Use the GPIO output pin schema for DC and RESET
            cv.Required("dc_pin"): gpio_output_pin_schema,
            cv.Optional("reset_pin"): gpio_output_pin_schema,
            cv.Optional(CONF_WIDTH, default=360): cv.uint16_t,
            cv.Optional(CONF_HEIGHT, default=360): cv.uint16_t,
        }
    )
    .extend(cv.polling_component_schema("1s"))
    .extend(spi.spi_device_schema(cs_pin_required=True))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    # Wire up the two pins
    dc = await cg.gpio_pin_expression(config["dc_pin"])
    cg.add(var.set_dc_pin_gpio(dc))
    if "reset_pin" in config:
        rst = await cg.gpio_pin_expression(config["reset_pin"])
        cg.add(var.set_reset_pin_gpio(rst))

    # Dimensions
    cg.add(var.set_dimensions(config[CONF_WIDTH], config[CONF_HEIGHT]))

    # Register SPI and display
    await spi.register_spi_device(var, config)
    await display.register_display(var, config)

    # If you allow a custom writer lambda, handle it here
    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayBuffer, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
