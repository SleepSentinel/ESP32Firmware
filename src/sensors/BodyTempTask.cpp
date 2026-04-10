#include "BodyTempTask.h"

#include <Arduino.h>

#include "BodyTempSensor.h"
#include "system/Config.h"
#include "system/Queues.h"

namespace {

bool isValidTemperature(float temp) {
  return temp >= 00.0f && temp <= 55.0f;
}

void logReading(float temperature) {
  Serial.print("Body temperature: ");
  Serial.print(temperature, 2);
  Serial.println(" C");
}

}  // namespace

void BodyTempTask(void* pvParameters) {
  (void)pvParameters;

  BodyTempSensor bodyTempSensor;
  bodyTempSensor.begin();

  TickType_t lastWakeTime = xTaskGetTickCount();

  while (true) {
    const float temperature = bodyTempSensor.read();

    if (isValidTemperature(temperature)) {
      logReading(temperature);

      if (bodyTempQueue == nullptr ||
          xQueueSend(bodyTempQueue, &temperature, 0) != pdPASS) {
        Serial.println("Body temp queue send failed");
      }
    } else {
      Serial.println("Invalid body temperature reading");
    }

    vTaskDelayUntil(
        &lastWakeTime,
        pdMS_TO_TICKS(SleepSentinel::Config::kSensorUpdateIntervalMs));
  }
}