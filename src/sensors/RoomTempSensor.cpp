#include "RoomTempSensor.h"

#include <math.h>

#include "system/Config.h"

RoomTempSensor::RoomTempSensor()
    : dht_(SleepSentinel::Config::kDhtDataPin,
           SleepSentinel::Config::kDhtSensorType) {}

void RoomTempSensor::begin() {
  dht_.begin();
}

RoomClimateReading RoomTempSensor::read() {
  const float humidity = dht_.readHumidity();
  const float temperatureC = dht_.readTemperature();

  if (isnan(humidity) || isnan(temperatureC)) {
    return {0.0f, 0.0f, false};
  }

  return {temperatureC, humidity, true};
}
