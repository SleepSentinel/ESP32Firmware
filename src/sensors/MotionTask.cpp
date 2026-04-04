#include "MotionTask.h"

#include <Arduino.h>

#include "MotionSensor.h"
#include "system/Config.h"
#include "system/Queues.h"

namespace {

void logReading(const MotionReading& reading) {
  Serial.print("Motion accel (m/s^2): x=");
  Serial.print(reading.accelX, 2);
  Serial.print(", y=");
  Serial.print(reading.accelY, 2);
  Serial.print(", z=");
  Serial.println(reading.accelZ, 2);
}

}  // namespace

void MotionTask(void* pvParameters) {
  (void)pvParameters;

  MotionSensor motionSensor;
  motionSensor.begin();

  TickType_t lastWakeTime = xTaskGetTickCount();

  while (true) {
    const MotionReading reading = motionSensor.read();

    if (reading.isValid) {
      logReading(reading);
    } else {
      Serial.println("Motion sensor read failed");
    }

    if (motionQueue == nullptr || xQueueSend(motionQueue, &reading, 0) != pdPASS) {
      Serial.println("Motion queue send failed");
    }

    vTaskDelayUntil(
        &lastWakeTime,
        pdMS_TO_TICKS(SleepSentinel::Config::kMotionSampleIntervalMs));
  }
}
