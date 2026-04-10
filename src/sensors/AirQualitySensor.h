#pragma once

#include <stdint.h>

struct AirQualityReading {
  uint16_t rawValue;
  bool isValid;
};

class AirQualitySensor {
 public:
  void begin();
  AirQualityReading read();
};
