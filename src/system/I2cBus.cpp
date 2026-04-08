#include "I2cBus.h"

#include <Wire.h>

#include "freertos/semphr.h"
#include "system/Config.h"

namespace {

SemaphoreHandle_t i2cBusMutex = nullptr;
bool i2cBusInitialized = false;

}  // namespace

bool initI2cBus() {
  if (i2cBusMutex == nullptr) {
    i2cBusMutex = xSemaphoreCreateMutex();
  }

  if (i2cBusMutex == nullptr) {
    return false;
  }

  if (!i2cBusInitialized) {
    i2cBusInitialized = Wire.begin(SleepSentinel::Config::kI2cSdaPin,
                                   SleepSentinel::Config::kI2cSclPin);
  }

  return i2cBusInitialized;
}

bool lockI2cBus(TickType_t timeout) {
  if (!initI2cBus()) {
    return false;
  }

  return xSemaphoreTake(i2cBusMutex, timeout) == pdTRUE;
}

void unlockI2cBus() {
  if (i2cBusMutex != nullptr) {
    xSemaphoreGive(i2cBusMutex);
  }
}
