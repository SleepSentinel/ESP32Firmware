#pragma once

#include <stdint.h>

class BodyTempSensor {
public:
  BodyTempSensor();

  void begin();
  float read();

private:
  uint8_t address_;
};