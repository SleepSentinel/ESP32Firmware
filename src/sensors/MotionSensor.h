#pragma once

#include <Adafruit_MPU6050.h>

struct MotionReading {
  float accelX;
  float accelY;
  float accelZ;
  bool isValid;
};

class MotionSensor {
 public:
  MotionSensor();

  bool begin();
  MotionReading read();

 private:
  Adafruit_MPU6050 mpu_;
  bool initialized_;
};
