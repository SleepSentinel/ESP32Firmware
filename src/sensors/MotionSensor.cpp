#include "MotionSensor.h"

#include <Arduino.h>
#include <Wire.h>

#include "system/Config.h"

MotionSensor::MotionSensor() : initialized_(false) {}

bool MotionSensor::begin() {
  Wire.begin(SleepSentinel::Config::kI2cSdaPin,
             SleepSentinel::Config::kI2cSclPin);

  if (!mpu_.begin(SleepSentinel::Config::kMpu6050Address)) {
    Serial.println("MPU6050 initialization failed");
    initialized_ = false;
    return false;
  }

  initialized_ = true;
  Serial.println("MPU6050 initialized");
  return true;
}

MotionReading MotionSensor::read() {
  if (!initialized_) {
    return {0.0f, 0.0f, 0.0f, false};
  }

  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  mpu_.getEvent(&accel, &gyro, &temp);

  return {
      accel.acceleration.x,
      accel.acceleration.y,
      accel.acceleration.z,
      true,
  };
}
