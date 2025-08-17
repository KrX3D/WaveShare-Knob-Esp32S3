import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID
from esphome import automation

DEPENDENCIES = ["i2c"]

drv2605_ns = cg.esphome_ns.namespace("drv2605")
DRV2605Component = drv2605_ns.class_("DRV2605Component", cg.Component, i2c.I2CDevice)

# Actions
PlayEffectAction = drv2605_ns.class_("PlayEffectAction", automation.Action)
StopAction = drv2605_ns.class_("StopAction", automation.Action)

CONF_DRV2605_ID = "drv2605_id"
CONF_EFFECT = "effect"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(DRV2605Component),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x5A))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)


@automation.register_action(
    "drv2605.play_effect",
    PlayEffectAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(DRV2605Component),
            cv.Required(CONF_EFFECT): cv.templatable(cv.int_range(min=1, max=123)),
        }
    ),
)
async def drv2605_play_effect_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = await cg.templatable(config[CONF_EFFECT], args, cg.uint8)
    cg.add(var.set_effect(template_))
    return var


@automation.register_action(
    "drv2605.stop",
    StopAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(DRV2605Component),
        }
    ),
)
async def drv2605_stop_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    return var
