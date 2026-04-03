#include "DataProcessor.h"

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

// System modules
#include "system/Queues.h"
#include "system/SystemState.h"

void DataProcessorTask(void *pvParameters) {
  int hr;
  int spo2;
  float bodyTemp;
  float roomTemp;
  bool motion;
  int sound;
  int air;

  while (1) {

    // Heart Rate
    if (xQueueReceive(hrQueue, &hr, 0)) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      systemState.heartRate = hr;
      systemState.alertHighHR = (hr > 160);
      systemState.alertLowHR = (hr < 60);

      xSemaphoreGive(stateMutex);
    }

    // SpO2
    if (xQueueReceive(spo2Queue, &spo2, 0)) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      systemState.spo2 = spo2;

      xSemaphoreGive(stateMutex);
    }

    // Body Temperature
    if (xQueueReceive(bodyTempQueue, &bodyTemp, 0)) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      systemState.bodyTemperature = bodyTemp;
      systemState.alertHighBodyTemp = (bodyTemp > 38.0);
      systemState.alertLowBodyTemp = (bodyTemp < 35.0);

      xSemaphoreGive(stateMutex);
    }

    // Room Temperature
    if (xQueueReceive(roomTempQueue, &roomTemp, 0)) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      systemState.roomTemperature = roomTemp;
      systemState.alertHighRoomTemp = (roomTemp > 30.0);
      systemState.alertLowRoomTemp = (roomTemp < 18.0);

      xSemaphoreGive(stateMutex);
    }

    // Motion
    if (xQueueReceive(motionQueue, &motion, 0)) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      systemState.isMoving = motion;

      xSemaphoreGive(stateMutex);
    }

    // Sound → Cry detection
    if (xQueueReceive(soundQueue, &sound, 0)) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      systemState.isCrying = (sound > 70); // threshold

      xSemaphoreGive(stateMutex);
    }

    // Air Quality
    if (xQueueReceive(airQueue, &air, 0)) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      systemState.airQuality = air;

      xSemaphoreGive(stateMutex);
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}