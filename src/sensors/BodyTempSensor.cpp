#include "BodyTempSensor.h"

#include <Wire.h>

#include "system/I2cBus.h"
#include "system/Config.h"

BodyTempSensor::BodyTempSensor()
    : address_(SleepSentinel::Config::kMax30205Address) {}

void BodyTempSensor::begin() {
  initI2cBus();
}

float BodyTempSensor::read() {
  if (!lockI2cBus(portMAX_DELAY)) {
    return 0.0f;
  }

  // Point to temperature register (0x00)
  Wire.beginTransmission(address_);
  Wire.write(0x00);

  if (Wire.endTransmission(true) != 0) {
    unlockI2cBus();
    return 0.0f;
  }

  // Request 2 bytes
  if (Wire.requestFrom(address_, (uint8_t)2) != 2) {
    unlockI2cBus();
    return 0.0f;
  }

  uint8_t msb = Wire.read();
  uint8_t lsb = Wire.read();
  unlockI2cBus();

  // Combine into 16-bit value
  int16_t raw = (msb << 8) | lsb;

  // Convert to Celsius (LSB = 1/256 °C)
  float temperature = raw * 0.00390625f;

  return temperature;
}
