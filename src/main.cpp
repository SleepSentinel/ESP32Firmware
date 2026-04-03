#include <Arduino.h>

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
  delay(200);
  Serial.println("SleepSentinel boot");

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
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
