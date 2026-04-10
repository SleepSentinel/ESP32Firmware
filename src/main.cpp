#include <Arduino.h>
#include <Wire.h>

#include "system/Config.h"
#include "system/Queues.h"
#include "system/SystemState.h"
#include "system/TaskManager.h"

namespace {

[[noreturn]] void haltWithError(const char* message) {
  Serial.println(message);
  while (true) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

}  // namespace

void setup() {
  Serial.begin(SleepSentinel::Config::kSerialBaudRate);
  delay(500); //for wifi connection
  Serial.println("SleepSentinel boot (RTOS mode)");

  Wire.begin(SleepSentinel::Config::kI2cSdaPin,
             SleepSentinel::Config::kI2cSclPin);

  if (!initQueues()) {
    haltWithError("Queue initialization failed");
  }

  if (!initSystemState()) {
    haltWithError("System state initialization failed");
  }

  if (!createTasks()) {
    haltWithError("Task creation failed");
  }

  Serial.println("Sprint 1 tasks started");
  Serial.println("All tasks including WebServerTask initialized");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
