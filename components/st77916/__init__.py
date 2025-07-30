import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import display, spi
from esphome.const import (
    CONF_COLOR_PALETTE,
    CONF_DC_PIN,
    CONF_ID,
    CONF_LAM,
    CONF_LAMBDA,
    CONF_MODEL,
    CONF_PAGES,
    CONF_RESET_PIN,
    CONF_DIMENSIONS,
    CONF_WIDTH,
    CONF_HEIGHT,
    CONF_ROTATION,
    CONF_MIRROR_X,
    CONF_MIRROR_Y,
    CONF_SWAP_XY,
    CONF_OFFSET_WIDTH,
    CONF_OFFSET_HEIGHT,
)

DEPENDENCIES = ["spi"]

st77916_ns = cg.esphome_ns.namespace("st77916")
ST77916Display = st77916_ns.class_(
    "ST77916Display", cg.PollingComponent, spi.SPIDevice, display.DisplayBuffer
)

ST77916Model = st77916_ns.enum("ST77916Model")

MODELS = {
    "ST77916": ST77916Model.ST77916_MODEL,
}

ST77916_MODEL = cv.enum(MODELS, upper=True)

CONF_BACKLIGHT_PIN = "backlight_pin"

CONFIG_SCHEMA = cv.All(
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(ST77916Display),
            cv.Required(CONF_MODEL): ST77916_MODEL,
            cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_BACKLIGHT_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_ROTATION, default=0): cv.int_range(min=0, max=3),
            cv.Optional(CONF_MIRROR_X, default=False): cv.boolean,
            cv.Optional(CONF_MIRROR_Y, default=False): cv.boolean,
            cv.Optional(CONF_SWAP_XY, default=False): cv.boolean,
            cv.Optional(CONF_OFFSET_WIDTH, default=0): cv.int_,
            cv.Optional(CONF_OFFSET_HEIGHT, default=0): cv.int_,
            cv.Optional(CONF_COLOR_PALETTE): cv.one_of("GRAYSCALE", "IMAGE_ADAPTIVE"),
        }
    )
    .extend(cv.polling_component_schema("1s"))
    .extend(spi.spi_device_schema(cs_pin_required=True)),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)

    cg.add(var.set_model(config[CONF_MODEL]))
    await display.register_display(var, config)

    if CONF_RESET_PIN in config:
        reset = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
        cg.add(var.set_reset_pin(reset))

    if CONF_BACKLIGHT_PIN in config:
        backlight = await cg.gpio_pin_expression(config[CONF_BACKLIGHT_PIN])
        cg.add(var.set_backlight_pin(backlight))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayBufferRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))

    cg.add(var.set_rotation(config[CONF_ROTATION]))
    cg.add(var.set_mirror_x(config[CONF_MIRROR_X]))
    cg.add(var.set_mirror_y(config[CONF_MIRROR_Y]))
    cg.add(var.set_swap_xy(config[CONF_SWAP_XY]))
    cg.add(var.set_offset_x(config[CONF_OFFSET_WIDTH]))
    cg.add(var.set_offset_y(config[CONF_OFFSET_HEIGHT]))

    if CONF_COLOR_PALETTE in config:
        cg.add(var.set_color_palette(config[CONF_COLOR_PALETTE]))