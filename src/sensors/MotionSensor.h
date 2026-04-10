#pragma once

#include <stdint.h>

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
  float accelLsbPerG() const;
  uint8_t detectedDeviceId_;
  bool initialized_;
};
