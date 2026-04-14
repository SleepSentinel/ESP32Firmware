#include "DataProcessor.h"

#include <Arduino.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "sensors/AirQualitySensor.h"
#include "sensors/MotionSensor.h"
#include "sensors/RoomTempSensor.h"
#include "system/Config.h"
#include "system/Queues.h"
#include "system/SystemState.h"

namespace {

struct AlertSnapshot {
  bool alertHighHR;
  bool alertLowHR;
  bool alertLowSpO2;
  bool alertHighBodyTemp;
  bool alertLowBodyTemp;
  bool alertHighRoomTemp;
  bool alertLowRoomTemp;
};

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

AlertSnapshot computeAlertSnapshot(const SystemState& state) {
  return {
      state.heartRate > 160,
      state.heartRate < 60,
      state.spo2 < 95,
      state.bodyTemperature > 38.0f,
      state.bodyTemperature < 35.0f,
      state.roomTemperature > 30.0f,
      state.roomTemperature < 18.0f,
  };
}

bool updateAlertStateLocked(SystemState* state) {
  const AlertSnapshot nextAlerts = computeAlertSnapshot(*state);
  const bool changed =
      state->alertHighHR != nextAlerts.alertHighHR ||
      state->alertLowHR != nextAlerts.alertLowHR ||
      state->alertLowSpO2 != nextAlerts.alertLowSpO2 ||
      state->alertHighBodyTemp != nextAlerts.alertHighBodyTemp ||
      state->alertLowBodyTemp != nextAlerts.alertLowBodyTemp ||
      state->alertHighRoomTemp != nextAlerts.alertHighRoomTemp ||
      state->alertLowRoomTemp != nextAlerts.alertLowRoomTemp;

  if (!changed) {
    return false;
  }

  state->alertHighHR = nextAlerts.alertHighHR;
  state->alertLowHR = nextAlerts.alertLowHR;
  state->alertLowSpO2 = nextAlerts.alertLowSpO2;
  state->alertHighBodyTemp = nextAlerts.alertHighBodyTemp;
  state->alertLowBodyTemp = nextAlerts.alertLowBodyTemp;
  state->alertHighRoomTemp = nextAlerts.alertHighRoomTemp;
  state->alertLowRoomTemp = nextAlerts.alertLowRoomTemp;
  state->alertVersion++;
  state->lastAlertChangeMs = static_cast<uint32_t>(millis());

  return true;
}

void logAlertTransition(const SystemState& state) {
  Serial.print("Alert version: ");
  Serial.print(state.alertVersion);
  Serial.print(" active=");

  bool first = true;
  auto appendAlert = [&first](const char* name) {
    if (!first) {
      Serial.print(",");
    }
    Serial.print(name);
    first = false;
  };

  if (state.alertHighHR) {
    appendAlert("highHR");
  }
  if (state.alertLowHR) {
    appendAlert("lowHR");
  }
  if (state.alertLowSpO2) {
    appendAlert("lowSpO2");
  }
  if (state.alertHighBodyTemp) {
    appendAlert("highBodyTemp");
  }
  if (state.alertLowBodyTemp) {
    appendAlert("lowBodyTemp");
  }
  if (state.alertHighRoomTemp) {
    appendAlert("highRoomTemp");
  }
  if (state.alertLowRoomTemp) {
    appendAlert("lowRoomTemp");
  }

  if (first) {
    Serial.print("none");
  }

  Serial.print(" changedAtMs=");
  Serial.println(state.lastAlertChangeMs);
}

AirQualityLevel classifyAirQuality(uint16_t rawValue) {
  if (rawValue <= 150) {
    return AirQualityLevel::kExcellent;
  }

  if (rawValue <= 300) {
    return AirQualityLevel::kGood;
  }

  if (rawValue <= 500) {
    return AirQualityLevel::kAverage;
  }

  if (rawValue <= 700) {
    return AirQualityLevel::kPoor;
  }

  if (rawValue <= 900) {
    return AirQualityLevel::kBad;
  }

  return AirQualityLevel::kDangerous;
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
  AirQualityReading airQualityReading = {0, false};
  RoomClimateReading roomClimateReading = {0.0f, 0.0f, false};
  MotionDetectionState motionDetectionState;

  while (true) {
    if (xQueueReceive(hrQueue, &hr, 0) == pdPASS) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      systemState.heartRate = hr;
      const bool alertsChanged = updateAlertStateLocked(&systemState);
      const SystemState alertSnapshot = systemState;

      Serial.print("Heart rate: ");
      Serial.print(systemState.heartRate);
      Serial.println(" bpm");

      xSemaphoreGive(stateMutex);

      if (alertsChanged) {
        logAlertTransition(alertSnapshot);
      }
    }

    if (xQueueReceive(spo2Queue, &spo2, 0) == pdPASS) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      systemState.spo2 = spo2;
      const bool alertsChanged = updateAlertStateLocked(&systemState);
      const SystemState alertSnapshot = systemState;

      Serial.print("SpO2: ");
      Serial.print(systemState.spo2);
      Serial.println(" %");

      xSemaphoreGive(stateMutex);

      if (alertsChanged) {
        logAlertTransition(alertSnapshot);
      }
    }

    if (xQueueReceive(bodyTempQueue, &bodyTemp, 0) == pdPASS) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      systemState.bodyTemperature = bodyTemp;
      const bool alertsChanged = updateAlertStateLocked(&systemState);
      const SystemState alertSnapshot = systemState;

      xSemaphoreGive(stateMutex);

      if (alertsChanged) {
        logAlertTransition(alertSnapshot);
      }
    }

    if (xQueueReceive(roomTempQueue, &roomTemp, 0) == pdPASS) {
      xSemaphoreTake(stateMutex, portMAX_DELAY);

      systemState.roomTemperature = roomTemp;
      const bool alertsChanged = updateAlertStateLocked(&systemState);
      const SystemState alertSnapshot = systemState;

      xSemaphoreGive(stateMutex);

      if (alertsChanged) {
        logAlertTransition(alertSnapshot);
      }
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

    if (xQueueReceive(airQueue, &airQualityReading, 0) == pdPASS) {
      if (airQualityReading.isValid) {
        setAirQualityReading(
            airQualityReading.rawValue,
            classifyAirQuality(airQualityReading.rawValue));
      } else {
        setAirQualityError();
      }
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
