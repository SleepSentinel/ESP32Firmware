#include "SystemState.h"

namespace {
SystemState defaultSystemState() {
  return {
      0,
      0,
      0.0f,
      0.0f,
      0.0f,
      false,
      0.0f,
      0.0f,
      0.0f,
      false,
      false,
      false,
      0,
      false,
      AirQualityLevel::kUnknown,
      false,
      false,
      false,
      false,
      false,
      false,
      false,
      0,
      0,
  };
}
}  // namespace

//Global Shared state by all tasks protected with a semaphore -> prevents race conditions 
SystemState systemState = defaultSystemState();
SemaphoreHandle_t stateMutex = nullptr;

// The Mutex Lock
bool initSystemState() {
  systemState = defaultSystemState();

  if (stateMutex == nullptr) {
    stateMutex = xSemaphoreCreateMutex();
  }

  return stateMutex != nullptr;
}

// Locks System State -> Updates shared state -> Unlocks System State
void setAirQualityReading(uint16_t rawValue, AirQualityLevel level) {
  if (stateMutex != nullptr) {
    xSemaphoreTake(stateMutex, portMAX_DELAY);
  }

  systemState.airQuality = rawValue;
  systemState.airQualitySensorOk = true;
  systemState.airQualityLevel = level;

  if (stateMutex != nullptr) {
    xSemaphoreGive(stateMutex);
  }
}

void setAirQualityError() {
  if (stateMutex != nullptr) {
    xSemaphoreTake(stateMutex, portMAX_DELAY);
  }

  systemState.airQualitySensorOk = false;
  systemState.airQualityLevel = AirQualityLevel::kUnknown;

  if (stateMutex != nullptr) {
    xSemaphoreGive(stateMutex);
  }
}

void setRoomClimateReading(float temperatureC, float humidityPercent) {
  if (stateMutex != nullptr) {
    xSemaphoreTake(stateMutex, portMAX_DELAY);
  }

  systemState.roomTemperature = temperatureC;
  systemState.roomHumidity = humidityPercent;
  systemState.roomSensorOk = true;

  if (stateMutex != nullptr) {
    xSemaphoreGive(stateMutex);
  }
}

void setRoomClimateError() {
  if (stateMutex != nullptr) {
    xSemaphoreTake(stateMutex, portMAX_DELAY);
  }

  systemState.roomSensorOk = false;

  if (stateMutex != nullptr) {
    xSemaphoreGive(stateMutex);
  }
}

// Waits forever until mutex is free -> data is not being change = safe to read
// returns latest copy of System State -> server gets a copy
SystemState getSystemStateSnapshot() {
  if (stateMutex != nullptr) {
    xSemaphoreTake(stateMutex, portMAX_DELAY);
  }

  const SystemState snapshot = systemState;

  if (stateMutex != nullptr) {
    xSemaphoreGive(stateMutex);
  }

  return snapshot;
}
