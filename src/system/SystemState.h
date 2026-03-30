#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

struct SystemState {
  int heartRate;
  int spo2;
  float bodyTemperature;
  float roomTemperature;
  bool isMoving;
  bool isCrying;
  int airQuality;

  // Alerts
  bool alertHighHR;
  bool alertLowHR;
  bool alertHighBodyTemp;
  bool alertLowBodyTemp;
  bool alertHighRoomTemp;
  bool alertLowRoomTemp;
};

extern SystemState systemState;
extern SemaphoreHandle_t stateMutex;

void initSystemState();