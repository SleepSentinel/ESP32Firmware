#include "BodyTempSensor.h"

#include <Wire.h>

#include "system/Config.h"
#include "system/I2cBus.h"

BodyTempSensor::BodyTempSensor()
    : address_(SleepSentinel::Config::kMax30205Address) {}

void BodyTempSensor::begin() {
  // I2C init is handled by I2cBus
  initI2cBus();
}

float BodyTempSensor::read() {
  // Try to lock I2C bus
  if (!lockI2cBus(pdMS_TO_TICKS(50))) {
    return 0.0f;
  }

  float temperature = 0.0f;

  do {
    // Point to temperature register (0x00)
    Wire.beginTransmission(address_);
    Wire.write(0x00);

    if (Wire.endTransmission(false) != 0) {
      break;
    }

    // Request 2 bytes
    if (Wire.requestFrom(address_, (uint8_t)2) != 2) {
      break;
    }

    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();

    int16_t raw = (msb << 8) | lsb;

    temperature = raw * 0.00390625f;

  } while (false);

  unlockI2cBus();

  return temperature;
}