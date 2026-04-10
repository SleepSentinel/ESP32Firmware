#include "AirQualityTask.h"

#include <Arduino.h>

#include "AirQualitySensor.h"
#include "system/Config.h"
#include "system/Queues.h"

namespace {

void logReading(const AirQualityReading& reading) {
  Serial.print("Air quality raw value: ");
  Serial.println(reading.rawValue);
}

}  // namespace

void AirQualityTask(void* pvParameters) {
  (void)pvParameters;

  AirQualitySensor airQualitySensor;
  airQualitySensor.begin();

  TickType_t lastWakeTime = xTaskGetTickCount();

  while (true) {
    const AirQualityReading reading = airQualitySensor.read();

    if (reading.isValid) {
      logReading(reading);
    } else {
      Serial.println("MQ135 read failed");
    }

    if (airQueue == nullptr || xQueueSend(airQueue, &reading, 0) != pdPASS) {
      Serial.println("Air quality queue send failed");
    }

    vTaskDelayUntil(
        &lastWakeTime,
        pdMS_TO_TICKS(SleepSentinel::Config::kAirQualitySamplingIntervalMs));
  }
}
