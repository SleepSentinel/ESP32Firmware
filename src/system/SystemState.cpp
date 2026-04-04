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
      false,
      false,
      0,
      false,
      false,
      false,
      false,
      false,
      false,
      false,
  };
}
}  // namespace

SystemState systemState = defaultSystemState();
SemaphoreHandle_t stateMutex = nullptr;

bool initSystemState() {
  systemState = defaultSystemState();

  if (stateMutex == nullptr) {
    stateMutex = xSemaphoreCreateMutex();
  }

  return stateMutex != nullptr;
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
