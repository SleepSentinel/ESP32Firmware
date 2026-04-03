#pragma once

#include <DHT.h>

struct RoomClimateReading {
  float temperatureC;
  float humidityPercent;
  bool isValid;
};

class RoomTempSensor {
 public:
  RoomTempSensor();

  void begin();
  RoomClimateReading read();

 private:
  DHT dht_;
};
