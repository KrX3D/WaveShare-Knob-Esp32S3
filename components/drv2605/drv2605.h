#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/i2c/i2c.h"

#define DRV2605_ADDR 0x5A
#define DRV2605_REG_STATUS 0x00
#define DRV2605_REG_MODE 0x01
#define DRV2605_REG_WAVESEQ1 0x04
#define DRV2605_REG_WAVESEQ2 0x05
#define DRV2605_REG_WAVESEQ3 0x06
#define DRV2605_REG_WAVESEQ4 0x07
#define DRV2605_REG_WAVESEQ5 0x08
#define DRV2605_REG_WAVESEQ6 0x09
#define DRV2605_REG_WAVESEQ7 0x0A
#define DRV2605_REG_WAVESEQ8 0x0B
#define DRV2605_REG_GO 0x0C
#define DRV2605_REG_OVERDRIVE 0x0D
#define DRV2605_REG_SUSTAINPOS 0x0E
#define DRV2605_REG_SUSTAINNEG 0x0F
#define DRV2605_REG_BREAK 0x10
#define DRV2605_REG_AUDIOCTRL 0x11
#define DRV2605_REG_AUDIOLVL 0x12
#define DRV2605_REG_AUDIOMAX 0x13
#define DRV2605_REG_RATEDV 0x16
#define DRV2605_REG_CLAMPV 0x17
#define DRV2605_REG_AUTOCALCOMP 0x18
#define DRV2605_REG_AUTOCALEMP 0x19
#define DRV2605_REG_FEEDBACK 0x1A
#define DRV2605_REG_CONTROL1 0x1B
#define DRV2605_REG_CONTROL2 0x1C
#define DRV2605_REG_CONTROL3 0x1D
#define DRV2605_REG_CONTROL4 0x1E
#define DRV2605_REG_VBAT 0x21
#define DRV2605_REG_LRARESON 0x22

#define DRV2605_MODE_INTTRIG 0x00
#define DRV2605_MODE_EXTTRIGEDGE 0x01
#define DRV2605_MODE_EXTTRIGLVL 0x02
#define DRV2605_MODE_PWMANALOG 0x03
#define DRV2605_MODE_AUDIOVIBE 0x04
#define DRV2605_MODE_REALTIME 0x05
#define DRV2605_MODE_DIAGNOS 0x06
#define DRV2605_MODE_AUTOCAL 0x07

#define DRV2605_LIBRARY_EMPTY 0x00
#define DRV2605_LIBRARY_TS2200A 0x01
#define DRV2605_LIBRARY_TS2200B 0x02
#define DRV2605_LIBRARY_TS2200C 0x03
#define DRV2605_LIBRARY_LRA 0x06

namespace esphome {
namespace drv2605 {

class DRV2605Component : public Component, public i2c::I2CDevice {
 public:
  void setup() override {
    ESP_LOGCONFIG("DRV2605", "Setting up DRV2605...");
    
    // Initialize I2C
    this->set_i2c_address(DRV2605_ADDR);
    
    // Reset device
    writeRegister8(DRV2605_REG_MODE, 0x80);
    delay(100);
    
    // Select library 1 (TS2200 A)
    writeRegister8(DRV2605_REG_WAVESEQ1, 1);
    writeRegister8(DRV2605_REG_WAVESEQ2, 0);
    
    // Set mode to internal trigger
    writeRegister8(DRV2605_REG_MODE, DRV2605_MODE_INTTRIG);
    
    // Select library
    selectLibrary(1);
    
    ESP_LOGCONFIG("DRV2605", "DRV2605 setup complete");
  }
  
  void selectLibrary(uint8_t lib) {
    writeRegister8(DRV2605_REG_WAVESEQ1, lib);
    writeRegister8(DRV2605_REG_WAVESEQ2, 0);
  }
  
  void setWaveform(uint8_t slot, uint8_t w) {
    writeRegister8(DRV2605_REG_WAVESEQ1 + slot, w);
  }
  
  void play_effect(uint8_t effect) {
    ESP_LOGD("DRV2605", "Playing haptic effect #%d: %s", effect, getEffectName(effect));
    
    // Set the waveform
    setWaveform(0, effect);  // Set effect
    setWaveform(1, 0);       // End waveform
    
    // Trigger playback
    writeRegister8(DRV2605_REG_GO, 1);
  }
  
  void stop() {
    writeRegister8(DRV2605_REG_GO, 0);
  }
  
  const char* getEffectName(uint8_t effect) {
    switch(effect) {
      case 1: return "Strong Click - 100%";
      case 2: return "Strong Click - 60%";
      case 3: return "Strong Click - 30%";
      case 4: return "Sharp Click - 100%";
      case 5: return "Sharp Click - 60%";
      case 6: return "Sharp Click - 30%";
      case 7: return "Soft Bump - 100%";
      case 8: return "Soft Bump - 60%";
      case 9: return "Soft Bump - 30%";
      case 10: return "Double Click - 100%";
      case 11: return "Double Click - 60%";
      case 12: return "Triple Click - 100%";
      case 13: return "Soft Fuzz - 60%";
      case 14: return "Strong Buzz - 100%";
      case 15: return "750 ms Alert 100%";
      case 16: return "1000 ms Alert 100%";
      case 47: return "Buzz 1 - 100%";
      case 48: return "Buzz 2 - 80%";
      case 49: return "Buzz 3 - 60%";
      case 50: return "Buzz 4 - 40%";
      case 51: return "Buzz 5 - 20%";
      case 52: return "Pulsing Strong 1 - 100%";
      case 53: return "Pulsing Strong 2 - 60%";
      case 118: return "Long buzz for programmatic stopping - 100%";
      case 119: return "Smooth Hum 1 (No kick or brake pulse) - 50%";
      case 120: return "Smooth Hum 2 (No kick or brake pulse) - 40%";
      case 121: return "Smooth Hum 3 (No kick or brake pulse) - 30%";
      case 122: return "Smooth Hum 4 (No kick or brake pulse) - 20%";
      case 123: return "Smooth Hum 5 (No kick or brake pulse) - 10%";
      default: return "Unknown Effect";
    }
  }

 private:
  void writeRegister8(uint8_t reg, uint8_t value) {
    if (!this->write_register(reg, &value, 1)) {
      ESP_LOGE("DRV2605", "Failed to write register 0x%02X", reg);
    }
  }
  
  uint8_t readRegister8(uint8_t reg) {
    uint8_t value = 0;
    if (!this->read_register(reg, &value, 1)) {
      ESP_LOGE("DRV2605", "Failed to read register 0x%02X", reg);
    }
    return value;
  }
};

// Action classes
template<typename... Ts> class PlayEffectAction : public Action<Ts...> {
 public:
  PlayEffectAction(DRV2605Component *parent) : parent_(parent) {}

  TEMPLATABLE_VALUE(uint8_t, effect)

  void play(Ts... x) override {
    auto effect = this->effect_.value(x...);
    this->parent_->play_effect(effect);
  }

 protected:
  DRV2605Component *parent_;
};

template<typename... Ts> class StopAction : public Action<Ts...> {
 public:
  StopAction(DRV2605Component *parent) : parent_(parent) {}

  void play(Ts... x) override {
    this->parent_->stop();
  }

 protected:
  DRV2605Component *parent_;
};

}  // namespace drv2605
}  // namespace esphome
