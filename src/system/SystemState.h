#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

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
void setRoomClimateReading(float temperatureC, float humidityPercent);
void setRoomClimateError();
SystemState getSystemStateSnapshot();
