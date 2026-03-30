#include <Arduino.h>

// System
#include "system/Queues.h"
#include "system/SystemState.h"
#include "system/TaskManager.h"

void setup() {
  // Initialize shared resources
  initQueues();
  initSystemState();

  // Create all FreeRTOS tasks
  createTasks();
}

void loop() {
  // Idle loop (FreeRTOS handles everything)
  vTaskDelay(pdMS_TO_TICKS(1000));
}
