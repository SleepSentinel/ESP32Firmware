#include "TaskManager.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "display/DisplayTask.h"
#include "processing/ProcessingTask.h"
#include "sensors/RoomTempTask.h"
#include "system/Config.h"

namespace {

configSTACK_DEPTH_TYPE stackBytesToWords(uint16_t stackBytes) {
  return stackBytes / sizeof(StackType_t);
}

}  // namespace

bool createTasks() {
  TaskHandle_t roomTempTaskHandle = nullptr;
  TaskHandle_t processingTaskHandle = nullptr;
  TaskHandle_t displayTaskHandle = nullptr;

  if (xTaskCreate(RoomTempTask,
                  "RoomTempTask",
                  stackBytesToWords(
                      SleepSentinel::Config::kRoomTempTaskStackBytes),
                  nullptr,
                  SleepSentinel::Config::kRoomTempTaskPriority,
                  &roomTempTaskHandle) != pdPASS) {
    return false;
  }

  if (xTaskCreate(ProcessingTask,
                  "ProcessingTask",
                  stackBytesToWords(
                      SleepSentinel::Config::kProcessingTaskStackBytes),
                  nullptr,
                  SleepSentinel::Config::kProcessingTaskPriority,
                  &processingTaskHandle) != pdPASS) {
    vTaskDelete(roomTempTaskHandle);
    return false;
  }

  if (xTaskCreate(DisplayTask,
                  "DisplayTask",
                  stackBytesToWords(
                      SleepSentinel::Config::kDisplayTaskStackBytes),
                  nullptr,
                  SleepSentinel::Config::kDisplayTaskPriority,
                  &displayTaskHandle) != pdPASS) {
    vTaskDelete(processingTaskHandle);
    vTaskDelete(roomTempTaskHandle);
    return false;
  }

  return true;
}
