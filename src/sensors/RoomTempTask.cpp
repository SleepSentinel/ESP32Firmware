#include "RoomTempTask.h"

#include <Arduino.h>

#include "RoomTempSensor.h"
#include "system/Config.h"
#include "system/Queues.h"

namespace {

void logReading(const RoomClimateReading& reading) {
  Serial.print("Room temperature: ");
  Serial.print(reading.temperatureC, 1);
  Serial.print(" C, humidity: ");
  Serial.print(reading.humidityPercent, 1);
  Serial.println(" %");
}

}  // namespace

void RoomTempTask(void* pvParameters) {
  (void)pvParameters;

  RoomTempSensor roomTempSensor;
  roomTempSensor.begin();

  TickType_t lastWakeTime = xTaskGetTickCount();

  while (true) {
    const RoomClimateReading reading = roomTempSensor.read();

    if (reading.isValid) {
      logReading(reading);
    } else {
      Serial.println("Room sensor read failed");
    }

    if (roomClimateQueue == nullptr ||
        xQueueSend(roomClimateQueue, &reading, 0) != pdPASS) {
      Serial.println("Room climate queue send failed");
    }

    vTaskDelayUntil(
        &lastWakeTime,
        pdMS_TO_TICKS(SleepSentinel::Config::kSensorUpdateIntervalMs));
  }
}
