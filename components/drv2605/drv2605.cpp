#include "drv2605.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace drv2605 {

static const char *const TAG = "drv2605";

void DRV2605Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up DRV2605...");
  
  // Reset device
  if (!this->write_register(DRV2605_REG_MODE, 0x80)) {
    ESP_LOGE(TAG, "Failed to reset device");
    this->mark_failed();
    return;
  }
  delayMicroseconds(100000);  // 100ms delay
  
  // Set mode to internal trigger
  if (!this->writeRegister8(DRV2605_REG_MODE, DRV2605_MODE_INTTRIG)) {
    ESP_LOGE(TAG, "Failed to set internal trigger mode");
    this->mark_failed();
    return;
  }
  
  // Select library 1 (TS2200 A)
  this->selectLibrary(1);
  
  ESP_LOGCONFIG(TAG, "DRV2605 setup complete");
}

void DRV2605Component::dump_config() {
  ESP_LOGCONFIG(TAG, "DRV2605:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with DRV2605 failed!");
  }
}

void DRV2605Component::selectLibrary(uint8_t lib) {
  this->writeRegister8(DRV2605_REG_WAVESEQ1, lib);
  this->writeRegister8(DRV2605_REG_WAVESEQ2, 0);
}

void DRV2605Component::setWaveform(uint8_t slot, uint8_t w) {
  this->writeRegister8(DRV2605_REG_WAVESEQ1 + slot, w);
}

void DRV2605Component::play_effect(uint8_t effect) {
  ESP_LOGD(TAG, "Playing haptic effect #%d: %s", effect, this->getEffectName(effect));
  
  // Set the waveform
  this->setWaveform(0, effect);  // Set effect
  this->setWaveform(1, 0);       // End waveform
  
  // Trigger playback
  this->writeRegister8(DRV2605_REG_GO, 1);
}

void DRV2605Component::stop() {
  ESP_LOGD(TAG, "Stopping haptic motor");
  this->writeRegister8(DRV2605_REG_GO, 0);
}

const char* DRV2605Component::getEffectName(uint8_t effect) {
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
    case 17: return "Strong Click 1 - 100%";
    case 18: return "Strong Click 2 - 80%";
    case 19: return "Strong Click 3 - 60%";
    case 20: return "Strong Click 4 - 30%";
    case 21: return "Medium Click 1 - 100%";
    case 22: return "Medium Click 2 - 80%";
    case 23: return "Medium Click 3 - 60%";
    case 24: return "Sharp Tick 1 - 100%";
    case 25: return "Sharp Tick 2 - 80%";
    case 26: return "Sharp Tick 3 - 60%";
    case 27: return "Short Double Click Strong 1 - 100%";
    case 28: return "Short Double Click Strong 2 - 80%";
    case 29: return "Short Double Click Strong 3 - 60%";
    case 30: return "Short Double Click Strong 4 - 30%";
    case 31: return "Short Double Click Medium 1 - 100%";
    case 32: return "Short Double Click Medium 2 - 80%";
    case 33: return "Short Double Click Medium 3 - 60%";
    case 34: return "Short Double Sharp Tick 1 - 100%";
    case 35: return "Short Double Sharp Tick 2 - 80%";
    case 36: return "Short Double Sharp Tick 3 - 60%";
    case 37: return "Long Double Sharp Click Strong 1 - 100%";
    case 38: return "Long Double Sharp Click Strong 2 - 80%";
    case 39: return "Long Double Sharp Click Strong 3 - 60%";
    case 40: return "Long Double Sharp Click Strong 4 - 30%";
    case 41: return "Long Double Sharp Click Medium 1 - 100%";
    case 42: return "Long Double Sharp Click Medium 2 - 80%";
    case 43: return "Long Double Sharp Click Medium 3 - 60%";
    case 44: return "Long Double Sharp Tick 1 - 100%";
    case 45: return "Long Double Sharp Tick 2 - 80%";
    case 46: return "Long Double Sharp Tick 3 - 60%";
    case 47: return "Buzz 1 - 100%";
    case 48: return "Buzz 2 - 80%";
    case 49: return "Buzz 3 - 60%";
    case 50: return "Buzz 4 - 40%";
    case 51: return "Buzz 5 - 20%";
    case 52: return "Pulsing Strong 1 - 100%";
    case 53: return "Pulsing Strong 2 - 60%";
    case 54: return "Pulsing Medium 1 - 100%";
    case 55: return "Pulsing Medium 2 - 60%";
    case 56: return "Pulsing Sharp 1 - 100%";
    case 57: return "Pulsing Sharp 2 - 60%";
    case 58: return "Transition Click 1 - 100%";
    case 59: return "Transition Click 2 - 80%";
    case 60: return "Transition Click 3 - 60%";
    case 61: return "Transition Click 4 - 40%";
    case 62: return "Transition Click 5 - 20%";
    case 63: return "Transition Click 6 - 10%";
    case 64: return "Transition Hum 1 - 100%";
    case 65: return "Transition Hum 2 - 80%";
    case 66: return "Transition Hum 3 - 60%";
    case 67: return "Transition Hum 4 - 40%";
    case 68: return "Transition Hum 5 - 20%";
    case 69: return "Transition Hum 6 - 10%";
    case 70: return "Transition Ramp Down Long Smooth 1 - 100 to 0%";
    case 71: return "Transition Ramp Down Long Smooth 2 - 100 to 0%";
    case 72: return "Transition Ramp Down Medium Smooth 1 - 100 to 0%";
    case 73: return "Transition Ramp Down Medium Smooth 2 - 100 to 0%";
    case 74: return "Transition Ramp Down Short Smooth 1 - 100 to 0%";
    case 75: return "Transition Ramp Down Short Smooth 2 - 100 to 0%";
    case 76: return "Transition Ramp Down Long Sharp 1 - 100 to 0%";
    case 77: return "Transition Ramp Down Long Sharp 2 - 100 to 0%";
    case 78: return "Transition Ramp Down Medium Sharp 1 - 100 to 0%";
    case 79: return "Transition Ramp Down Medium Sharp 2 - 100 to 0%";
    case 80: return "Transition Ramp Down Short Sharp 1 - 100 to 0%";
    case 81: return "Transition Ramp Down Short Sharp 2 - 100 to 0%";
    case 82: return "Transition Ramp Up Long Smooth 1 - 0 to 100%";
    case 83: return "Transition Ramp Up Long Smooth 2 - 0 to 100%";
    case 84: return "Transition Ramp Up Medium Smooth 1 - 0 to 100%";
    case 85: return "Transition Ramp Up Medium Smooth 2 - 0 to 100%";
    case 86: return "Transition Ramp Up Short Smooth 1 - 0 to 100%";
    case 87: return "Transition Ramp Up Short Smooth 2 - 0 to 100%";
    case 88: return "Transition Ramp Up Long Sharp 1 - 0 to 100%";
    case 89: return "Transition Ramp Up Long Sharp 2 - 0 to 100%";
    case 90: return "Transition Ramp Up Medium Sharp 1 - 0 to 100%";
    case 91: return "Transition Ramp Up Medium Sharp 2 - 0 to 100%";
    case 92: return "Transition Ramp Up Short Sharp 1 - 0 to 100%";
    case 93: return "Transition Ramp Up Short Sharp 2 - 0 to 100%";
    case 94: return "Transition Ramp Down Long Smooth 1 - 50 to 0%";
    case 95: return "Transition Ramp Down Long Smooth 2 - 50 to 0%";
    case 96: return "Transition Ramp Down Medium Smooth 1 - 50 to 0%";
    case 97: return "Transition Ramp Down Medium Smooth 2 - 50 to 0%";
    case 98: return "Transition Ramp Down Short Smooth 1 - 50 to 0%";
    case 99: return "Transition Ramp Down Short Smooth 2 - 50 to 0%";
    case 100: return "Transition Ramp Down Long Sharp 1 - 50 to 0%";
    case 101: return "Transition Ramp Down Long Sharp 2 - 50 to 0%";
    case 102: return "Transition Ramp Down Medium Sharp 1 - 50 to 0%";
    case 103: return "Transition Ramp Down Medium Sharp 2 - 50 to 0%";
    case 104: return "Transition Ramp Down Short Sharp 1 - 50 to 0%";
    case 105: return "Transition Ramp Down Short Sharp 2 - 50 to 0%";
    case 106: return "Transition Ramp Up Long Smooth 1 - 0 to 50%";
    case 107: return "Transition Ramp Up Long Smooth 2 - 0 to 50%";
    case 108: return "Transition Ramp Up Medium Smooth 1 - 0 to 50%";
    case 109: return "Transition Ramp Up Medium Smooth 2 - 0 to 50%";
    case 110: return "Transition Ramp Up Short Smooth 1 - 0 to 50%";
    case 111: return "Transition Ramp Up Short Smooth 2 - 0 to 50%";
    case 112: return "Transition Ramp Up Long Sharp 1 - 0 to 50%";
    case 113: return "Transition Ramp Up Long Sharp 2 - 0 to 50%";
    case 114: return "Transition Ramp Up Medium Sharp 1 - 0 to 50%";
    case 115: return "Transition Ramp Up Medium Sharp 2 - 0 to 50%";
    case 116: return "Transition Ramp Up Short Sharp 1 - 0 to 50%";
    case 117: return "Transition Ramp Up Short Sharp 2 - 0 to 50%";
    case 118: return "Long buzz for programmatic stopping - 100%";
    case 119: return "Smooth Hum 1 (No kick or brake pulse) - 50%";
    case 120: return "Smooth Hum 2 (No kick or brake pulse) - 40%";
    case 121: return "Smooth Hum 3 (No kick or brake pulse) - 30%";
    case 122: return "Smooth Hum 4 (No kick or brake pulse) - 20%";
    case 123: return "Smooth Hum 5 (No kick or brake pulse) - 10%";
    default: return "Unknown Effect";
  }
}

bool DRV2605Component::writeRegister8(uint8_t reg, uint8_t value) {
  return this->write_register(reg, &value, 1);
}

uint8_t DRV2605Component::readRegister8(uint8_t reg) {
  uint8_t value = 0;
  if (!this->read_register(reg, &value, 1)) {
    ESP_LOGE(TAG, "Failed to read register 0x%02X", reg);
  }
  return value;
}

}  // namespace drv2605
}  // namespace esphome
