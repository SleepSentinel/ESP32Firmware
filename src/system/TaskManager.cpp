#include "TaskManager.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "server/WiFiManager.h"
#include "server/WebSocketServer.h"
#include "system/Config.h"

// ⏸ Disabled for now (will re-enable later)
#include "sensors/RoomTempTask.h"
#include "processing/DataProcessor.h"
#include "display/DisplayTask.h"

namespace {

configSTACK_DEPTH_TYPE stackBytesToWords(uint16_t stackBytes) {
  return stackBytes / sizeof(StackType_t);
}

WiFiManager wifiManager;
WebSocketServer wsServer(SleepSentinel::Config::WS_PORT);

void WebServerTask(void* pvParameters) {
  Serial.println("Starting WiFi...");

  wifiManager.begin();

  if (wifiManager.isConnected()) {

    wsServer.begin();

    while (true) {
      wsServer.run();   // non-blocking
      vTaskDelay(pdMS_TO_TICKS(50));
    }

  } else {
    Serial.println("WiFi failed, WebSocket server not started");
  }

  vTaskDelete(nullptr);
}

} // namespace

bool createTasks() {
  // COMMENTED OUT TASKS for testing only (enable later after PR)
  /*
  TaskHandle_t roomTempTaskHandle = nullptr;
  TaskHandle_t processingTaskHandle = nullptr;
  TaskHandle_t displayTaskHandle = nullptr;
  */
  TaskHandle_t webServerTaskHandle = nullptr;

  // (WiFi + WebSocket)
  if (xTaskCreatePinnedToCore(
          WebServerTask,
          "WebServerTask",
          stackBytesToWords(
              SleepSentinel::Config::kWebServerTaskStackBytes),
          nullptr,
          SleepSentinel::Config::kWebServerTaskPriority,
          &webServerTaskHandle,
          0) != pdPASS) {

    Serial.println("Failed to create WebServerTask");
    return false;
  }

  // COMMENTED OUT TASKS for testing only (enable later after PR)

  /*
  if (xTaskCreate(RoomTempTask,
                  "RoomTempTask",
                  stackBytesToWords(
                      SleepSentinel::Config::kRoomTempTaskStackBytes),
                  nullptr,
                  SleepSentinel::Config::kRoomTempTaskPriority,
                  &roomTempTaskHandle) != pdPASS) {
    return false;
  }

  if (xTaskCreate(DataProcessorTask,
                  "DataProcessor",
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
  */

  return true;
}