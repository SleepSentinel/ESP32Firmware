#pragma once

#include <LiquidCrystal_I2C.h>

#include "system/SystemState.h"

class LcdDisplay {
 public:
  LcdDisplay();

  void begin();
  void showStartup();
  void render(const SystemState& state);

 private:
  LiquidCrystal_I2C lcd_;
  char lastLine0_[17];
  char lastLine1_[17];
  char lastLine2_[17];
  char lastLine3_[17];

  void updateLine(uint8_t row, const char* nextLine, char* cachedLine);
};
