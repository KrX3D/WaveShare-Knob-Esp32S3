#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/log.h"

#define DRV2605_ADDR 0x5A
#define DRV2605_REG_STATUS 0x00
#define DRV2605_REG_MODE 0x01
#define DRV2605_REG_RTPIN 0x02
#define DRV2605_REG_LIBRARY 0x03  // Added missing library register
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
  void setup() override;
  void dump_config() override;
  
  void selectLibrary(uint8_t lib);
  void setWaveform(uint8_t slot, uint8_t w);
  void play_effect(uint8_t effect);
  void stop();
  const char* getEffectName(uint8_t effect);

 private:
  bool writeRegister8(uint8_t reg, uint8_t value);
  uint8_t readRegister8(uint8_t reg);
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