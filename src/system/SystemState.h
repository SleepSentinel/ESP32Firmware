#pragma once
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

enum class AirQualityLevel : uint8_t {
  kUnknown,
  kExcellent,
  kGood,
  kAverage,
  kPoor,
  kBad,
  kDangerous,
};

struct SystemState {
  int heartRate;
  int spo2;
  float bodyTemperature;
  float roomTemperature;
  float roomHumidity;
  bool roomSensorOk;
  float motionAccelX;
  float motionAccelY;
  float motionAccelZ;
  bool motionSensorOk;
  bool isMoving;
  bool isCrying;
  int airQuality;
  bool airQualitySensorOk;
  AirQualityLevel airQualityLevel;

  // Alerts
  bool alertHighHR;
  bool alertLowHR;
  bool alertLowSpO2;
  bool alertHighBodyTemp;
  bool alertLowBodyTemp;
  bool alertHighRoomTemp;
  bool alertLowRoomTemp;
};

extern SystemState systemState;
extern SemaphoreHandle_t stateMutex;

bool initSystemState();
void setAirQualityReading(uint16_t rawValue, AirQualityLevel level);
void setAirQualityError();
void setRoomClimateReading(float temperatureC, float humidityPercent);
void setRoomClimateError();
SystemState getSystemStateSnapshot();
