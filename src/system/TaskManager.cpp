#include "TaskManager.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "server/WiFiManager.h"
#include "server/WebSocketServer.h"
#include "system/Config.h"

#include "sensors/RoomTempTask.h"
#include "processing/DataProcessor.h"
#include "display/DisplayTask.h"

namespace {

// Free RTOS Expects stack size in words, it is defined in bytes in Config.h
// this converts it bytes -> words
configSTACK_DEPTH_TYPE stackBytesToWords(uint16_t stackBytes) {
  return stackBytes / sizeof(StackType_t);
}

WiFiManager wifiManager;
WebSocketServer wsServer(SleepSentinel::Config::WS_PORT);

void WebServerTask(void* pvParameters) {
  Serial.println("Starting WiFi...");

  wifiManager.begin();

  // Keeps trying until WiFi connects
  while (!wifiManager.isConnected()) {
    Serial.println("Waiting for WiFi connection...");
    vTaskDelay(pdMS_TO_TICKS(2000));  // retry every 2s
  }

  Serial.println("WiFi connected");

  // Start WebSocket server AFTER WiFi is ready
  wsServer.begin();

  // Main loop (runs forever)
  while (true) {
    wsServer.run();   // non-blocking
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

} // namespace

bool createTasks() {
  TaskHandle_t roomTempTaskHandle = nullptr;
  TaskHandle_t processingTaskHandle = nullptr;
  TaskHandle_t displayTaskHandle = nullptr;
  TaskHandle_t webServerTaskHandle = nullptr;

  // (WiFi + WebSocket)
  // ESP32 is dual core -> Pinned to Core pins this task to Core 0
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

  // Rest of Tasks share Core 1
  // RoomTemp task reads from DHT sensor
  if (xTaskCreate(RoomTempTask,
                  "RoomTempTask",
                  stackBytesToWords(
                      SleepSentinel::Config::kRoomTempTaskStackBytes),
                  nullptr,
                  SleepSentinel::Config::kRoomTempTaskPriority,
                  &roomTempTaskHandle) != pdPASS) {
    vTaskDelete(webServerTaskHandle);
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
    vTaskDelete(webServerTaskHandle);
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
    vTaskDelete(webServerTaskHandle);
    return false;
  }

  return true;
}