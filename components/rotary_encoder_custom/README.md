# Rotary Encoder Custom Component

A simple, reliable rotary encoder component for ESPHome using polling with optimized debouncing.

## Features

- ✅ **Simple polling-based detection** - Reliable and easy to understand
- ✅ **Optimized debouncing** - Fast response with noise filtering
- ✅ **Min/Max limits** - Bounds checking
- ✅ **Restore modes** - Remember position across reboots
- ✅ **Reset pin support** - Hardware reset capability
- ✅ **Clockwise/Anticlockwise triggers** - Separate automation triggers
- ✅ **set_value action** - Programmatically set counter

## Configuration

### Basic Example

```yaml
sensor:
  - platform: rotary_encoder_custom
    name: "Rotary Encoder"
    id: my_encoder
    pin_a: GPIO8
    pin_b: GPIO7
```

### Complete Example with All Options

```yaml
sensor:
  - platform: rotary_encoder_custom
    name: "Rotary Encoder"
    id: my_encoder
    pin_a: GPIO8
    pin_b: GPIO7
    
    # Optional reset pin - counter resets to 0 when HIGH
    pin_reset: GPIO9
    
    # Min/max limits (optional)
    min_value: 0
    max_value: 100
    
    # Publish initial value on boot (default: false)
    publish_initial_value: true
    
    # Restore mode (default: RESTORE_DEFAULT_ZERO)
    # RESTORE_DEFAULT_ZERO: restore last value, or 0 if no saved value
    # ALWAYS_ZERO: always start at 0
    restore_mode: RESTORE_DEFAULT_ZERO
    
    # Separate triggers for each direction
    on_clockwise:
      - logger.log: "Turned clockwise!"
    
    on_anticlockwise:
      - logger.log: "Turned counter-clockwise!"
```

## Configuration Options

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `pin_a` | Pin | **Required** | First encoder pin (A phase) |
| `pin_b` | Pin | **Required** | Second encoder pin (B phase) |
| `pin_reset` | Pin | Optional | Reset pin - counter resets when HIGH |
| `min_value` | int | -2147483648 | Minimum counter value |
| `max_value` | int | 2147483647 | Maximum counter value |
| `publish_initial_value` | bool | false | Publish value on boot |
| `restore_mode` | enum | RESTORE_DEFAULT_ZERO | How to restore counter on boot |

### Restore Modes

- **RESTORE_DEFAULT_ZERO**: Try to restore last value from flash, default to 0 if not found
- **ALWAYS_ZERO**: Always start at 0, don't restore

## Actions

### Set Value

Set the encoder to a specific value:

```yaml
button:
  - platform: template
    name: "Set to 50"
    on_press:
      - sensor.rotary_encoder_custom.set_value:
          id: my_encoder
          value: 50
```

With lambda:

```yaml
on_...:
  - sensor.rotary_encoder_custom.set_value:
      id: my_encoder
      value: !lambda 'return id(some_sensor).state;'
```

## Triggers

### on_clockwise

Fires once for each clockwise step:

```yaml
sensor:
  - platform: rotary_encoder_custom
    # ...
    on_clockwise:
      - logger.log: "Clockwise turn detected"
      - homeassistant.action:
          action: media_player.volume_up
          data:
            entity_id: media_player.living_room
```

### on_anticlockwise

Fires once for each counter-clockwise step:

```yaml
sensor:
  - platform: rotary_encoder_custom
    # ...
    on_anticlockwise:
      - logger.log: "Counter-clockwise turn detected"
      - homeassistant.action:
          action: media_player.volume_down
          data:
            entity_id: media_player.living_room
```

## Use Cases

### Volume Control (0-100)
```yaml
sensor:
  - platform: rotary_encoder_custom
    name: "Volume"
    id: volume
    pin_a: GPIO8
    pin_b: GPIO7
    min_value: 0
    max_value: 100
    on_value:
      - homeassistant.action:
          action: media_player.volume_set
          data:
            entity_id: media_player.spotify
            volume_level: !lambda 'return x / 100.0;'
```

### Menu Navigation
```yaml
sensor:
  - platform: rotary_encoder_custom
    name: "Menu Position"
    id: menu_index
    pin_a: GPIO8
    pin_b: GPIO7
    min_value: 0
    max_value: 9  # 10 menu items
    restore_mode: ALWAYS_ZERO
    on_value:
      - lambda: |-
          update_display_menu((int)x);
```

### Media Player Seeking
```yaml
sensor:
  - platform: rotary_encoder_custom
    name: "Media Seek"
    id: seek_encoder
    pin_a: GPIO8
    pin_b: GPIO7
    on_clockwise:
      - homeassistant.action:
          action: media_player.media_seek
          data:
            entity_id: media_player.spotify
            seek_position: !lambda 'return id(media_position).state + 5;'
    on_anticlockwise:
      - homeassistant.action:
          action: media_player.media_seek
          data:
            entity_id: media_player.spotify
            seek_position: !lambda 'return id(media_position).state - 5;'
```

### Smart Thermostat
```yaml
sensor:
  - platform: rotary_encoder_custom
    name: "Target Temperature"
    id: temp_target
    pin_a: GPIO8
    pin_b: GPIO7
    min_value: 16
    max_value: 30
    restore_mode: RESTORE_DEFAULT_ZERO  # Remember last setting
    filters:
      - multiply: 0.5  # 0.5°C increments
```

### Hardware Reset Pin Example
```yaml
sensor:
  - platform: rotary_encoder_custom
    name: "Position"
    id: position
    pin_a: GPIO8
    pin_b: GPIO7
    pin_reset: GPIO9  # When this pin goes HIGH, counter resets to 0
    min_value: 0
    max_value: 1000
```

## Technical Details

### Simple Polling Algorithm

This component uses a straightforward polling approach with optimized debouncing:

- **Separate channel processing**: Each encoder pin (A and B) is monitored independently
- **Edge detection**: Counts rising edges on each channel after debouncing
- **Debounce counter**: Requires 2 stable LOW readings before accepting a rising edge
- **Direction**: A-phase rising = clockwise, B-phase rising = counter-clockwise

### How It Works

1. Every `loop()` iteration reads both encoder pins
2. For each pin that changed:
   - If LOW: increment debounce counter
   - If HIGH (rising edge) and debounce reached: count step and update counter
3. Simple, predictable behavior with fast response

### Performance Characteristics

- **Debounce time**: ~2ms at typical loop rates
- **Response time**: Excellent for both slow and fast rotation
- **CPU usage**: Minimal - just two pin reads per loop
- **Memory**: ~50 bytes RAM

### Why Polling Instead of Interrupts?

While interrupts seem ideal for encoder reading, this simple encoder works better with polling because:
- The debounce logic requires multiple sequential reads
- Polling at ESPHome's loop rate (~1kHz) is fast enough
- Simpler code = more reliable
- No ISR complexity or timing issues

## Troubleshooting

**Missing steps at high rotation speed:**
- This is normal - the encoder is being rotated faster than the debounce can track
- For most applications (volume, menu navigation), this isn't an issue

**Counter bounces or jumps erratically:**
- Check for loose connections
- Ensure good ground connection
- Try adding 0.1µF capacitors across encoder pins to ground

**Wrong direction:**
- Swap `pin_a` and `pin_b` in configuration

**Counter doesn't restore after reboot:**
- Check `restore_mode: RESTORE_DEFAULT_ZERO` is set
- Ensure flash wear leveling is working (see ESPHome docs)

## Compatibility

- ✅ ESP32 (all variants)
- ✅ ESP8266  
- ✅ RP2040
- ✅ Any ESPHome-supported platform

## Notes

- Unit of measurement is "steps" (not a physical unit)
- Counter wraps at INT32_MIN/INT32_MAX if no limits set
- Hardware pull-ups are automatically enabled on ESP platforms
- Optimized for typical mechanical rotary encoders with detents
