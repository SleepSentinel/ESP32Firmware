#include "LcdDisplay.h"

#include <Wire.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "system/Config.h"
#include "system/I2cBus.h"

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

const char* airQualityLevelText(AirQualityLevel level) {
  switch (level) {
    case AirQualityLevel::kExcellent:
      return "Excellent";
    case AirQualityLevel::kGood:
      return "Good";
    case AirQualityLevel::kAverage:
      return "Average";
    case AirQualityLevel::kPoor:
      return "Poor";
    case AirQualityLevel::kBad:
      return "Bad";
    case AirQualityLevel::kDangerous:
      return "Dangerous";
    case AirQualityLevel::kUnknown:
    default:
      return "Unknown";
  }
}

}  // namespace

LcdDisplay::LcdDisplay()
    : lcd_(SleepSentinel::Config::kLcdAddress,
           SleepSentinel::Config::kLcdColumns,
           SleepSentinel::Config::kLcdRows),
      lastLine0_{0},
      lastLine1_{0},
      lastLine2_{0},
      lastLine3_{0} {}

void LcdDisplay::begin() {
  if (!lockI2cBus(portMAX_DELAY)) {
    return;
  }

  lcd_.init();
  lcd_.backlight();
  lcd_.clear();

  unlockI2cBus();
}

void LcdDisplay::showStartup() {
  char line0[17];
  char line1[17];
  char line2[17];
  char line3[17];

  formatLine(line0, sizeof(line0), "SleepSentinel");
  formatLine(line1, sizeof(line1), "Starting...");
  formatLine(line2, sizeof(line2), "");
  formatLine(line3, sizeof(line3), "");

  updateLine(0, line0, lastLine0_);
  updateLine(1, line1, lastLine1_);
  updateLine(2, line2, lastLine2_);
  updateLine(3, line3, lastLine3_);
}

void LcdDisplay::render(const SystemState& state) {
  char line0[17];
  char line1[17];
  char line2[17];
  char line3[17];

  if (!state.roomSensorOk) {
    formatLine(line0, sizeof(line0), "Room sensor err");
    formatLine(line1, sizeof(line1), "Check sensor");
  } else {
    formatLine(line0, sizeof(line0), "Temp: %4.1fC", state.roomTemperature);
    formatLine(line1, sizeof(line1), "Hum : %4.1f%%", state.roomHumidity);
  }

  if (!state.airQualitySensorOk) {
    formatLine(line2, sizeof(line2), "AirQ: ERR");
    formatLine(line3, sizeof(line3), "Unknown");
  } else {
    formatLine(line2, sizeof(line2), "AirQ: %d", state.airQuality);
    formatLine(line3, sizeof(line3), "%s",
               airQualityLevelText(state.airQualityLevel));
  }

  updateLine(0, line0, lastLine0_);
  updateLine(1, line1, lastLine1_);
  updateLine(2, line2, lastLine2_);
  updateLine(3, line3, lastLine3_);
}

void LcdDisplay::updateLine(uint8_t row,
                            const char* nextLine,
                            char* cachedLine) {
  if (strncmp(nextLine, cachedLine, 16) == 0) {
    return;
  }

  if (!lockI2cBus(portMAX_DELAY)) {
    return;
  }

  lcd_.setCursor(0, row);
  lcd_.print(nextLine);
  unlockI2cBus();

  strncpy(cachedLine, nextLine, 17);
  cachedLine[16] = '\0';
}
