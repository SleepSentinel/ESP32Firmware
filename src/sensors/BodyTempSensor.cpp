#include "BodyTempSensor.h"

#include <Wire.h>

#include "system/Config.h"

BodyTempSensor::BodyTempSensor()
    : address_(SleepSentinel::Config::kMax30205Address) {}

void BodyTempSensor::begin() {
  Wire.begin(SleepSentinel::Config::kI2cSdaPin,
             SleepSentinel::Config::kI2cSclPin);
}

float BodyTempSensor::read() {
  // Point to temperature register (0x00)
  Wire.beginTransmission(address_);
  Wire.write(0x00);

  if (Wire.endTransmission(false) != 0) {
    return 0.0f;
  }

  // Request 2 bytes
  Wire.requestFrom(address_, (uint8_t)2);

  if (Wire.available() < 2) {
    return 0.0f;
  }

  uint8_t msb = Wire.read();
  uint8_t lsb = Wire.read();

  // Combine into 16-bit value
  int16_t raw = (msb << 8) | lsb;

  // Convert to Celsius (LSB = 1/256 °C)
  float temperature = raw * 0.00390625f;

  return temperature;
}