#pragma once

#include <stdint.h>
#include <Wire.h>
#include "MAX30105.h"

struct PulseOximetryReading {
  int heartRate;
  int spo2;
  bool heartRateValid;
  bool spo2Valid;
};

class PulseOximeterSensor {
 public:
  PulseOximeterSensor();

  bool begin();
  PulseOximetryReading read();

 private:
  static constexpr int kBufferLength = 100;
  static constexpr int kSlideWindowStep = 25;

  MAX30105 sensor_;
  uint32_t irBuffer_[kBufferLength];
  uint32_t redBuffer_[kBufferLength];
  bool initialized_;
  bool initialBufferFilled_;

  bool waitForNewSample();
  bool fillBufferRange(int startIndex, int endIndex);
  PulseOximetryReading calculateReading();
};
