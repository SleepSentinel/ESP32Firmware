#include "DataProcessor.h"

#include <Arduino.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "sensors/MotionSensor.h"
#include "sensors/RoomTempSensor.h"
#include "system/Config.h"
#include "system/Queues.h"
#include "system/SystemState.h"

namespace {

struct MotionDetectionState {
  MotionReading previousReading = {0.0f, 0.0f, 0.0f, false};
  bool hasPreviousReading = false;
  bool isMoving = false;
  uint8_t consecutiveAboveThreshold = 0;
  uint8_t consecutiveBelowThreshold = 0;
};

float motionDelta(const MotionReading& current, const MotionReading& previous) {
  return fabsf(current.accelX - previous.accelX) +
         fabsf(current.accelY - previous.accelY) +
         fabsf(current.accelZ - previous.accelZ);
}

bool detectMotion(const MotionReading& reading,
                  MotionDetectionState* detectionState) {
  const SleepSentinel::Config::MotionDetectionParameters parameters =
      SleepSentinel::Config::kMotionDetectionParameters;

  if (!detectionState->hasPreviousReading) {
    detectionState->previousReading = reading;
    detectionState->hasPreviousReading = true;
    detectionState->isMoving = false;
    detectionState->consecutiveAboveThreshold = 0;
    detectionState->consecutiveBelowThreshold = 0;
    return false;
  }

  const float delta = motionDelta(reading, detectionState->previousReading);
  detectionState->previousReading = reading;

  if (delta >= parameters.threshold) {
    detectionState->consecutiveAboveThreshold++;
    detectionState->consecutiveBelowThreshold = 0;
  } else {
    detectionState->consecutiveBelowThreshold++;
    detectionState->consecutiveAboveThreshold = 0;
  }

  if (detectionState->consecutiveAboveThreshold >= parameters.debounceCount) {
    detectionState->isMoving = true;
  }

  if (detectionState->consecutiveBelowThreshold >= parameters.debounceCount) {
    detectionState->isMoving = false;
  }

  return detectionState->isMoving;
}

void resetMotionDetectionState(MotionDetectionState* detectionState) {
  detectionState->previousReading = {0.0f, 0.0f, 0.0f, false};
  detectionState->hasPreviousReading = false;
  detectionState->isMoving = false;
  detectionState->consecutiveAboveThreshold = 0;
  detectionState->consecutiveBelowThreshold = 0;
}

void logMotionState(bool isMoving) {
  Serial.print("Motion: ");
  Serial.println(isMoving ? "true" : "false");
}

}  // namespace

void DataProcessorTask(void* pvParameters) {
  (void)pvParameters;

  int hr = 0;
  int spo2 = 0;
  float bodyTemp = 0.0f;
  float roomTemp = 0.0f;
  MotionReading motionReading = {0.0f, 0.0f, 0.0f, false};
  int sound = 0;
  int air = 0;
  RoomClimateReading roomClimateReading = {0.0f, 0.0f, false};
  MotionDetectionState motionDetectionState;

  while (true) {
    if (xQueueReceive(hrQueue, &hr, 0) == pdPASS) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      systemState.heartRate = hr;
      systemState.alertHighHR = (hr > 160);
      systemState.alertLowHR = (hr < 60);

      Serial.print("Heart rate: ");
      Serial.print(systemState.heartRate);
      Serial.println(" bpm");

      xSemaphoreGive(stateMutex);
    }

    if (xQueueReceive(spo2Queue, &spo2, 0) == pdPASS) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      systemState.spo2 = spo2;
      systemState.alertLowSpO2 = (spo2 < 95);

      Serial.print("SpO2: ");
      Serial.print(systemState.spo2);
      Serial.println(" %");

      xSemaphoreGive(stateMutex);
    }

    if (xQueueReceive(bodyTempQueue, &bodyTemp, 0) == pdPASS) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      systemState.bodyTemperature = bodyTemp;
      systemState.alertHighBodyTemp = (bodyTemp > 38.0f);
      systemState.alertLowBodyTemp = (bodyTemp < 35.0f);

      xSemaphoreGive(stateMutex);
    }

    if (xQueueReceive(roomTempQueue, &roomTemp, 0) == pdPASS) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      systemState.roomTemperature = roomTemp;
      systemState.alertHighRoomTemp = (roomTemp > 30.0f);
      systemState.alertLowRoomTemp = (roomTemp < 18.0f);

      xSemaphoreGive(stateMutex);
    }

    if (xQueueReceive(motionQueue, &motionReading, 0) == pdPASS) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      const bool previousMotionState = systemState.isMoving;

      if (motionReading.isValid) {
        systemState.motionAccelX = motionReading.accelX;
        systemState.motionAccelY = motionReading.accelY;
        systemState.motionAccelZ = motionReading.accelZ;
        systemState.motionSensorOk = true;
        systemState.isMoving =
            detectMotion(motionReading, &motionDetectionState);
      } else {
        systemState.motionAccelX = 0.0f;
        systemState.motionAccelY = 0.0f;
        systemState.motionAccelZ = 0.0f;
        systemState.motionSensorOk = false;
        systemState.isMoving = false;
        resetMotionDetectionState(&motionDetectionState);
      }

      const bool currentMotionState = systemState.isMoving;
      xSemaphoreGive(stateMutex);

      if (currentMotionState != previousMotionState) {
        logMotionState(currentMotionState);
      }
    }

    if (xQueueReceive(soundQueue, &sound, 0) == pdPASS) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      systemState.isCrying = (sound > 70);

      xSemaphoreGive(stateMutex);
    }

    if (xQueueReceive(airQueue, &air, 0) == pdPASS) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      systemState.airQuality = air;

      xSemaphoreGive(stateMutex);
    }

    if (xQueueReceive(roomClimateQueue, &roomClimateReading, 0) == pdPASS) {
      if (roomClimateReading.isValid) {
        setRoomClimateReading(roomClimateReading.temperatureC,
                              roomClimateReading.humidityPercent);
      } else {
        setRoomClimateError();
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
