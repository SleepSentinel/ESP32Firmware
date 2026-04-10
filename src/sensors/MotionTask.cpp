#include "MotionTask.h"

#include <Arduino.h>

#include "MotionSensor.h"
#include "system/Config.h"
#include "system/Queues.h"

namespace {

MotionReading invalidReading() {
  return {0.0f, 0.0f, 0.0f, false};
}

}  // namespace

void MotionTask(void* pvParameters) {
  (void)pvParameters;

  MotionSensor motionSensor;
  bool motionSensorReady = motionSensor.begin();

  if (motionSensorReady) {
    Serial.println("Motion sensor initialization succeeded");
  } else {
    Serial.println("Motion sensor initialization failed, will retry");
  }

  TickType_t lastWakeTime = xTaskGetTickCount();

  while (true) {
    MotionReading reading = invalidReading();

    if (!motionSensorReady) {
      motionSensorReady = motionSensor.begin();

      if (motionSensorReady) {
        Serial.println("Motion sensor initialization succeeded");
      } else {
        Serial.println("Motion sensor initialization retry failed");
      }
    }

    if (motionSensorReady) {
      reading = motionSensor.read();
      if (!reading.isValid) {
        Serial.println("Motion sensor read failed after successful init");
      }
    }

    if (motionQueue == nullptr || xQueueSend(motionQueue, &reading, 0) != pdPASS) {
      Serial.println("Motion queue send failed");
    }

    vTaskDelayUntil(
        &lastWakeTime,
        pdMS_TO_TICKS(SleepSentinel::Config::kMotionSampleIntervalMs));
  }
}
