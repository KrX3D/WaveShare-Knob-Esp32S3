# WaveShare ESP32-S3 1.8inch Knob Display / Guition JC3636K518C-I-YR1 - Knob-ESP32S3

> ESPHome configuration for the WaveShare / Guition knob display on an ESP32-S3.

---

## Status

**WIP** — mostly functional, audio still under test.

---

## Features

- ✅ **ST77916 display** — custom external component via MIPI SPI
- ✅ **CST816 touchscreen** — tap, swipe left/right support
- ✅ **Rotary encoder** — custom polling-based component with debouncing, min/max limits, restore modes, clockwise/anticlockwise triggers, and `set_value` action
- ✅ **LVGL UI** — multi-page interface (clock/weather page + media player page)
- ✅ **Media player integration** — track name, artist, album, volume arc, seek via encoder, album art via `online_image`
- ✅ **Battery monitoring** — ADC voltage with calibrated percentage, arc indicator with color coding (green/orange/red)
- ✅ **SD card** — read, write, append, delete, list, format via `sd_mmc_card` (4-bit MMC mode)
- ✅ **Haptic motor (DRV2605)** — LRA motor with effect library, triggered on encoder turns
- ✅ **Backlight control** — PWM dimming with screensaver timeout (LVGL snow effect on sleep)
- 🧪 **Audio and microphone** — experimental, not yet enabled by default

---

## Known issues

- Audio and microphone code are still experimental / under test — contributions welcome

If you encounter other issues or want to help, please open an issue or a PR.

---

## Project structure

```
example/esphome/
├── SmartKnob.yaml          # Top-level config, pulls in all packages below
└── SmartKnob/
    ├── esp-config.yaml     # Board, framework, PSRAM, UART
    ├── ha-config.yaml      # WiFi, API, OTA, time, debug sensors
    ├── waveshare.yaml      # Display, touchscreen, SPI/I2C, backlight, screensaver
    ├── lvgl.yaml           # Fonts, globals, battery arc
    ├── mediaplayer.yaml    # HA media player sensors, volume, seek, album art scripts
    ├── adc_battery.yaml    # ADC battery voltage + percentage sensor
    ├── sd_card.yaml        # SD MMC card component + file operation buttons
    ├── drv2605.yaml        # Haptic motor driver
    └── audio.yaml          # (experimental, disabled by default)

components/
└── rotary_encoder_custom/  # Custom rotary encoder sensor component
```

---

## Rotary encoder component

The `rotary_encoder_custom` component is a simple polling-based encoder with optimised debouncing. It lives in `components/` and is picked up automatically as an external component.

```yaml
sensor:
  - platform: rotary_encoder_custom
    name: "Rotary Encoder"
    id: rotary_counter
    pin_a: GPIO8
    pin_b: GPIO7
    min_value: 0
    max_value: 100
    restore_mode: RESTORE_DEFAULT_ZERO  # or ALWAYS_ZERO
    publish_initial_value: true
    on_clockwise:
      - logger.log: "CW"
    on_anticlockwise:
      - logger.log: "CCW"
```

Set value programmatically:

```yaml
- sensor.rotary_encoder_custom.set_value:
    id: rotary_counter
    value: 0
```

See [`components/rotary_encoder_custom/README.md`](components/rotary_encoder_custom/README.md) for full documentation.

---

## Related links

### SDMMC Component
- KrX3D fork (extended): https://github.com/KrX3D/esphome_sd_card/
- Original: https://github.com/n-serrette/esphome_sd_card

### Haptic Motor Driver Component
https://github.com/RAR/esphome-drv2605

### Hardware references & community
- Really good hardware info: https://github.com/muness/roon-knob/tree/master/docs
- ESPHome discussion: https://github.com/orgs/esphome/discussions/3253
- Home Assistant community: https://community.home-assistant.io/t/display-knob/905249
- Alternative full-peripheral config: https://github.com/nkinnan/Waveshare-ESP32-S3-Knob-Touch-LCD-1.8_and_Guition-K5-Knob-Series-JC3636K518

---

## Credits

Big thanks to **RealDeco** and **RAR** for help and contributions — audio and mic integration are currently being tested.