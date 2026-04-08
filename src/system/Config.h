#pragma once

#include <Arduino.h>
#include <stdint.h>

namespace SleepSentinel {
namespace Config {

enum class MotionSensitivity : uint8_t {
  Low,
  Medium,
  High,
};

struct MotionDetectionParameters {
  float threshold;
  uint8_t debounceCount;
};

constexpr MotionSensitivity kMotionSensitivity = MotionSensitivity::Medium;

constexpr MotionDetectionParameters motionDetectionParametersFor(
    MotionSensitivity sensitivity) {
  return sensitivity == MotionSensitivity::Low
             ? MotionDetectionParameters{1.5f, 3}
             : (sensitivity == MotionSensitivity::High
                    ? MotionDetectionParameters{0.6f, 2}
                    : MotionDetectionParameters{1.0f, 2});
}

constexpr MotionDetectionParameters kMotionDetectionParameters =
    motionDetectionParametersFor(kMotionSensitivity);

constexpr uint32_t kSerialBaudRate = 115200;
constexpr uint8_t kDhtDataPin = 4;
constexpr uint8_t kDhtSensorType = 22;
constexpr uint8_t kI2cSdaPin = 21;
constexpr uint8_t kI2cSclPin = 22;
constexpr uint8_t kMpu6050Address = 0x68;
constexpr uint8_t kLcdAddress = 0x27;
constexpr uint8_t kLcdColumns = 16;
constexpr uint8_t kLcdRows = 2;
constexpr unsigned long kStartupScreenDurationMs = 1000;
constexpr unsigned long kSensorUpdateIntervalMs = 2000;
constexpr unsigned long kPulseOximeterRateUpdateIntervalMs = 1000;
constexpr unsigned long kMotionSampleIntervalMs = 250;
constexpr unsigned long kDisplayUpdateIntervalMs = 500;
constexpr uint8_t kRoomClimateQueueLength = 4;
constexpr uint16_t kPulseOximeterTaskStackBytes = 8192;
constexpr uint16_t kRoomTempTaskStackBytes = 4096;
constexpr uint16_t kMotionTaskStackBytes = 4096;
constexpr uint16_t kProcessingTaskStackBytes = 4096;
constexpr uint16_t kDisplayTaskStackBytes = 4096;
constexpr uint8_t kPulseOximeterTaskPriority = 1;

// Task priorities to be updated.
constexpr uint8_t kRoomTempTaskPriority = 1;
constexpr uint8_t kMotionTaskPriority = 1;
constexpr uint8_t kProcessingTaskPriority = 1;
constexpr uint8_t kDisplayTaskPriority = 1;

// Server info.
// DO NOT PUSH YOUR WiFi info here; update locally only.
constexpr const char* WIFI_SSID = "*********";
constexpr const char* WIFI_PASSWORD = "*********";

constexpr uint16_t kWebServerTaskStackBytes = 8192;
constexpr UBaseType_t kWebServerTaskPriority = 1;
constexpr uint16_t WS_PORT = 80;

}  // namespace Config
}  // namespace SleepSentinel
