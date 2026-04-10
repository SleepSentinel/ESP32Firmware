#include "PulseOximeterTask.h"

#include <Arduino.h>

#include "PulseOximeterSensor.h"
#include "system/Config.h"
#include "system/Queues.h"

void PulseOximeterTask(void* pvParameters) {
  (void)pvParameters;

  PulseOximeterSensor pulseOximeterSensor;
  if (!pulseOximeterSensor.begin()) {
    Serial.println("MAX30102 initialization failed");
    vTaskDelete(nullptr);
    return;
  }

  TickType_t lastWakeTime = xTaskGetTickCount();

  while (true) {
    const PulseOximetryReading reading = pulseOximeterSensor.read();

    if (!reading.heartRateValid && !reading.spo2Valid) {
      Serial.println("MAX30102 reading invalid");
    }

    if (reading.heartRateValid) {
      if (hrQueue == nullptr ||
          xQueueSend(hrQueue, &reading.heartRate, 0) != pdPASS) {
        Serial.println("Heart rate queue send failed");
      }
    }

    if (reading.spo2Valid) {
      if (spo2Queue == nullptr ||
          xQueueSend(spo2Queue, &reading.spo2, 0) != pdPASS) {
        Serial.println("SpO2 queue send failed");
      }
    }

    vTaskDelayUntil(
        &lastWakeTime,
        pdMS_TO_TICKS(SleepSentinel::Config::kPulseOximeterRateUpdateIntervalMs));
  }
}
