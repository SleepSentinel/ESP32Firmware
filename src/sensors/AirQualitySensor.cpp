#include "AirQualitySensor.h"

#include <Arduino.h>

#include "system/Config.h"

namespace {

constexpr int kEsp32AdcMaxValue = 4095;

}  // namespace

void AirQualitySensor::begin() {
  pinMode(SleepSentinel::Config::kMq135AnalogPin, INPUT);
}

AirQualityReading AirQualitySensor::read() {
  const int rawValue = analogRead(SleepSentinel::Config::kMq135AnalogPin);

  if (rawValue < 0 || rawValue > kEsp32AdcMaxValue) {
    return {0, false};
  }

  return {static_cast<uint16_t>(rawValue), true};
}
