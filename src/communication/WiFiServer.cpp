#include <WebServer.h>
#include <WiFi.h>

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

// System
#include "system/SystemState.h"

// WiFi credentials (move to Config.h later)
const char *ssid = "YOUR_WIFI_NAME";
const char *password = "YOUR_WIFI_PASSWORD";

// Create server on port 80
WebServer server(80);

// Function to handle /data endpoint
void handleData() {
  String json;

  xSemaphoreTake(stateMutex, portMAX_DELAY);

  json = "{";
  json += "\"heartRate\":" + String(systemState.heartRate) + ",";
  json += "\"spo2\":" + String(systemState.spo2) + ",";
  json += "\"bodyTemperature\":" + String(systemState.bodyTemperature) + ",";
  json += "\"roomTemperature\":" + String(systemState.roomTemperature) + ",";
  json +=
      "\"isMoving\":" + String(systemState.isMoving ? "true" : "false") + ",";
  json +=
      "\"isCrying\":" + String(systemState.isCrying ? "true" : "false") + ",";
  json += "\"airQuality\":" + String(systemState.airQuality);
  json += "}";

  xSemaphoreGive(stateMutex);

  server.send(200, "application/json", json);
}

// Initialize WiFi + server
void initWiFiServer() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(pdMS_TO_TICKS(500));
  }

  // Define endpoint
  server.on("/data", handleData);

  server.begin();
}

// FreeRTOS communication task
void CommunicationTask(void *pvParameters) {

  initWiFiServer();

  while (1) {
    server.handleClient(); // handle incoming requests

    vTaskDelay(pdMS_TO_TICKS(10)); // small delay (important)
  }
}