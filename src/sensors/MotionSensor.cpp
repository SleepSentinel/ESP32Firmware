#include "MotionSensor.h"

#include <Arduino.h>
#include <Wire.h>

#include "system/Config.h"
#include "system/I2cBus.h"

namespace {

constexpr uint8_t kPowerManagement1Register = 0x6B;
constexpr uint8_t kAccelConfigRegister = 0x1C;
constexpr uint8_t kWhoAmIRegister = 0x75;
constexpr uint8_t kAccelDataRegister = 0x3B;
constexpr float kMpu6050AccelLsbPerG = 16384.0f;
constexpr float kIcm20689AccelLsbPerG = 8192.0f;
constexpr float kMetersPerSecondSquaredPerG = 9.80665f;

bool readRegister(uint8_t deviceAddress, uint8_t registerAddress, uint8_t* value) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(registerAddress);
  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  if (Wire.requestFrom(static_cast<int>(deviceAddress), 1) != 1) {
    return false;
  }

  *value = Wire.read();
  return true;
}

bool readRegisters(uint8_t deviceAddress,
                   uint8_t registerAddress,
                   uint8_t* buffer,
                   size_t length) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(registerAddress);
  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  const size_t bytesRead =
      Wire.requestFrom(static_cast<int>(deviceAddress), static_cast<int>(length));
  if (bytesRead != length) {
    return false;
  }

  for (size_t i = 0; i < length; ++i) {
    buffer[i] = Wire.read();
  }

  return true;
}

bool writeRegister(uint8_t deviceAddress, uint8_t registerAddress, uint8_t value) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(registerAddress);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

bool isSupportedMotionDeviceId(uint8_t deviceId) {
  switch (deviceId) {
    case 0x68:
    case 0x69:
    case 0x70:
    case 0x71:
    case 0x73:
    case 0x98:
      return true;
    default:
      return false;
  }
}

const char* motionDeviceName(uint8_t deviceId) {
  switch (deviceId) {
    case 0x98:
      return "ICM-20689-compatible";
    case 0x68:
    case 0x69:
    case 0x70:
    case 0x71:
    case 0x73:
      return "MPU6050-compatible";
    default:
      return "unknown";
  }
}

void logHexValue(const char* prefix, uint8_t value) {
  Serial.print(prefix);
  if (value < 0x10) {
    Serial.print('0');
  }
  Serial.println(value, HEX);
}

}  // namespace

MotionSensor::MotionSensor() : detectedDeviceId_(0), initialized_(false) {}

bool MotionSensor::begin() {
  if (!lockI2cBus(portMAX_DELAY)) {
    initialized_ = false;
    Serial.println("Motion sensor I2C lock failed during init");
    return false;
  }

  uint8_t detectedDeviceId = 0;
  const bool whoAmIReadSucceeded =
      readRegister(SleepSentinel::Config::kMpu6050Address,
                   kWhoAmIRegister,
                   &detectedDeviceId);

  if (whoAmIReadSucceeded) {
    detectedDeviceId_ = detectedDeviceId;
    logHexValue("Motion sensor WHO_AM_I=0x", detectedDeviceId_);
    Serial.print("Motion sensor detected as ");
    Serial.println(motionDeviceName(detectedDeviceId_));
  } else {
    Serial.println("Motion sensor WHO_AM_I read failed");
  }

  const bool beginSucceeded =
      whoAmIReadSucceeded && isSupportedMotionDeviceId(detectedDeviceId_) &&
      writeRegister(SleepSentinel::Config::kMpu6050Address,
                    kPowerManagement1Register,
                    0x01) &&
      writeRegister(SleepSentinel::Config::kMpu6050Address,
                    kAccelConfigRegister,
                    0x00);
  unlockI2cBus();

  if (beginSucceeded) {
    delay(100);
  } else if (whoAmIReadSucceeded && !isSupportedMotionDeviceId(detectedDeviceId_)) {
    Serial.println("Motion sensor identity is not MPU6050-compatible");
  } else {
    Serial.println("Motion sensor register initialization failed");
  }

  initialized_ = beginSucceeded;
  return beginSucceeded;
}

MotionReading MotionSensor::read() {
  if (!initialized_) {
    return {0.0f, 0.0f, 0.0f, false};
  }

  if (!lockI2cBus(portMAX_DELAY)) {
    return {0.0f, 0.0f, 0.0f, false};
  }

  uint8_t rawAccelBytes[6] = {0};
  const bool readSucceeded =
      readRegisters(SleepSentinel::Config::kMpu6050Address,
                    kAccelDataRegister,
                    rawAccelBytes,
                    sizeof(rawAccelBytes));
  unlockI2cBus();

  if (!readSucceeded) {
    return {0.0f, 0.0f, 0.0f, false};
  }

  const int16_t rawAccelX =
      static_cast<int16_t>((rawAccelBytes[0] << 8) | rawAccelBytes[1]);
  const int16_t rawAccelY =
      static_cast<int16_t>((rawAccelBytes[2] << 8) | rawAccelBytes[3]);
  const int16_t rawAccelZ =
      static_cast<int16_t>((rawAccelBytes[4] << 8) | rawAccelBytes[5]);

  return {
      (static_cast<float>(rawAccelX) / accelLsbPerG()) *
          kMetersPerSecondSquaredPerG,
      (static_cast<float>(rawAccelY) / accelLsbPerG()) *
          kMetersPerSecondSquaredPerG,
      (static_cast<float>(rawAccelZ) / accelLsbPerG()) *
          kMetersPerSecondSquaredPerG,
      true,
  };
}

float MotionSensor::accelLsbPerG() const {
  if (detectedDeviceId_ == 0x98) {
    return kIcm20689AccelLsbPerG;
  }

  return kMpu6050AccelLsbPerG;
}
