#include "LcdDisplay.h"

#include <Wire.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "system/Config.h"

namespace {

void formatLine(char* destination, size_t size, const char* format, ...) {
  char rawLine[17] = {0};

  va_list args;
  va_start(args, format);
  vsnprintf(rawLine, sizeof(rawLine), format, args);
  va_end(args);

  memset(destination, ' ', size - 1);
  destination[size - 1] = '\0';

  const size_t rawLength = strlen(rawLine);
  memcpy(destination, rawLine, rawLength);
}

}  // namespace

LcdDisplay::LcdDisplay()
    : lcd_(SleepSentinel::Config::kLcdAddress,
           SleepSentinel::Config::kLcdColumns,
           SleepSentinel::Config::kLcdRows),
      lastLine0_{0},
      lastLine1_{0} {}

void LcdDisplay::begin() {
  Wire.begin(SleepSentinel::Config::kI2cSdaPin,
             SleepSentinel::Config::kI2cSclPin);
  lcd_.init();
  lcd_.backlight();
  lcd_.clear();
}

void LcdDisplay::showStartup() {
  char line0[17];
  char line1[17];

  formatLine(line0, sizeof(line0), "SleepSentinel");
  formatLine(line1, sizeof(line1), "Starting...");

  updateLine(0, line0, lastLine0_);
  updateLine(1, line1, lastLine1_);
}

void LcdDisplay::render(const SystemState& state) {
  char line0[17];
  char line1[17];

  if (!state.roomSensorOk) {
    formatLine(line0, sizeof(line0), "Room sensor err");
    formatLine(line1, sizeof(line1), "Check sensor");
  } else {
    formatLine(line0, sizeof(line0), "Temp: %4.1f C", state.roomTemperature);
    formatLine(line1, sizeof(line1), "Hum : %4.1f %%", state.roomHumidity);
  }

  updateLine(0, line0, lastLine0_);
  updateLine(1, line1, lastLine1_);
}

void LcdDisplay::updateLine(uint8_t row,
                            const char* nextLine,
                            char* cachedLine) {
  if (strncmp(nextLine, cachedLine, 16) == 0) {
    return;
  }

  lcd_.setCursor(0, row);
  lcd_.print(nextLine);
  strncpy(cachedLine, nextLine, 17);
  cachedLine[16] = '\0';
}
