#include "ProcessingTask.h"

#include <Arduino.h>

#include "sensors/RoomTempSensor.h"
#include "system/Queues.h"
#include "system/SystemState.h"

void ProcessingTask(void* pvParameters) {
  (void)pvParameters;

  RoomClimateReading reading = {0.0f, 0.0f, false};

  while (true) {
    if (roomClimateQueue == nullptr) {
      vTaskDelay(pdMS_TO_TICKS(100));
      continue;
    }

    if (xQueueReceive(roomClimateQueue, &reading, portMAX_DELAY) != pdPASS) {
      continue;
    }

    if (reading.isValid) {
      setRoomClimateReading(reading.temperatureC, reading.humidityPercent);
    } else {
      setRoomClimateError();
    }
  }
}
