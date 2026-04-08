#include "TaskManager.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "display/DisplayTask.h"
#include "processing/DataProcessor.h"
#include "server/WiFiManager.h"
#include "server/WebSocketServer.h"
#include "sensors/AirQualityTask.h"
#include "sensors/MotionTask.h"
#include "sensors/PulseOximeterTask.h"
#include "sensors/RoomTempTask.h"
#include "system/Config.h"

namespace {

// FreeRTOS expects stack size in words; Config.h defines it in bytes.
configSTACK_DEPTH_TYPE stackBytesToWords(uint16_t stackBytes) {
  return stackBytes / sizeof(StackType_t);
}

WiFiManager wifiManager;
WebSocketServer wsServer(SleepSentinel::Config::WS_PORT);

void WebServerTask(void* pvParameters) {
  (void)pvParameters;

  Serial.println("Starting WiFi...");

  wifiManager.begin();

  while (!wifiManager.isConnected()) {
    Serial.println("Waiting for WiFi connection...");
    wifiManager.begin();
    vTaskDelay(pdMS_TO_TICKS(2000));
  }

  Serial.println("WiFi connected");

  wsServer.begin();

  while (true) {
    wsServer.run();
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

}  // namespace

bool createTasks() {
  TaskHandle_t pulseOximeterTaskHandle = nullptr;
  TaskHandle_t roomTempTaskHandle = nullptr;
  TaskHandle_t motionTaskHandle = nullptr;
  TaskHandle_t airQualityTaskHandle = nullptr;
  TaskHandle_t processingTaskHandle = nullptr;
  TaskHandle_t displayTaskHandle = nullptr;
  TaskHandle_t webServerTaskHandle = nullptr;

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

  if (xTaskCreate(PulseOximeterTask,
                  "PulseOximeterTask",
                  stackBytesToWords(
                      SleepSentinel::Config::kPulseOximeterTaskStackBytes),
                  nullptr,
                  SleepSentinel::Config::kPulseOximeterTaskPriority,
                  &pulseOximeterTaskHandle) != pdPASS) {
    vTaskDelete(webServerTaskHandle);
    return false;
  }

  if (xTaskCreate(RoomTempTask,
                  "RoomTempTask",
                  stackBytesToWords(
                      SleepSentinel::Config::kRoomTempTaskStackBytes),
                  nullptr,
                  SleepSentinel::Config::kRoomTempTaskPriority,
                  &roomTempTaskHandle) != pdPASS) {
    vTaskDelete(pulseOximeterTaskHandle);
    vTaskDelete(webServerTaskHandle);
    return false;
  }

  if (xTaskCreate(MotionTask,
                  "MotionTask",
                  stackBytesToWords(
                      SleepSentinel::Config::kMotionTaskStackBytes),
                  nullptr,
                  SleepSentinel::Config::kMotionTaskPriority,
                  &motionTaskHandle) != pdPASS) {
    vTaskDelete(roomTempTaskHandle);
    vTaskDelete(pulseOximeterTaskHandle);
    vTaskDelete(webServerTaskHandle);
    return false;
  }

  if (xTaskCreate(AirQualityTask,
                  "AirQualityTask",
                  stackBytesToWords(
                      SleepSentinel::Config::kAirQualityTaskStackBytes),
                  nullptr,
                  SleepSentinel::Config::kAirQualityTaskPriority,
                  &airQualityTaskHandle) != pdPASS) {
    vTaskDelete(motionTaskHandle);
    vTaskDelete(roomTempTaskHandle);
    vTaskDelete(pulseOximeterTaskHandle);
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
    vTaskDelete(airQualityTaskHandle);
    vTaskDelete(motionTaskHandle);
    vTaskDelete(roomTempTaskHandle);
    vTaskDelete(pulseOximeterTaskHandle);
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
    vTaskDelete(airQualityTaskHandle);
    vTaskDelete(motionTaskHandle);
    vTaskDelete(roomTempTaskHandle);
    vTaskDelete(pulseOximeterTaskHandle);
    vTaskDelete(webServerTaskHandle);
    return false;
  }

  return true;
}
