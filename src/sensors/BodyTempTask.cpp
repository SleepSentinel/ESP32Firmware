#include "BodyTempTask.h"

#include <Arduino.h>

#include "BodyTempSensor.h"
#include "system/Config.h"
#include "system/Queues.h"

namespace {

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

    // Log reading (for debugging, consistent with other sensors)
    logReading(temperature);

    // Send to queue
    if (bodyTempQueue == nullptr ||
        xQueueSend(bodyTempQueue, &temperature, 0) != pdPASS) {
      Serial.println("Body temp queue send failed");
    }

    // Maintain fixed sampling interval
    vTaskDelayUntil(
        &lastWakeTime,
        pdMS_TO_TICKS(SleepSentinel::Config::kSensorUpdateIntervalMs));
  }
}