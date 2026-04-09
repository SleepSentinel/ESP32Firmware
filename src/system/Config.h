#pragma once

#include <stdint.h>

namespace SleepSentinel {
namespace Config {

constexpr uint32_t kSerialBaudRate = 115200;
constexpr uint8_t kDhtDataPin = 4;
constexpr uint8_t kDhtSensorType = 22;
constexpr uint8_t kI2cSdaPin = 21;
constexpr uint8_t kI2cSclPin = 22;
constexpr uint8_t kLcdAddress = 0x27;
constexpr uint8_t kLcdColumns = 16;
constexpr uint8_t kLcdRows = 2;
constexpr unsigned long kStartupScreenDurationMs = 1000;
constexpr unsigned long kSensorUpdateIntervalMs = 2000;
constexpr unsigned long kDisplayUpdateIntervalMs = 500;
constexpr uint8_t kRoomClimateQueueLength = 4;
constexpr uint16_t kRoomTempTaskStackBytes = 4096;
constexpr uint16_t kProcessingTaskStackBytes = 4096;
constexpr uint16_t kDisplayTaskStackBytes = 4096;

//Task Priorities to be updated
constexpr uint8_t kRoomTempTaskPriority = 1;
constexpr uint8_t kProcessingTaskPriority = 1;
constexpr uint8_t kDisplayTaskPriority = 1;

// server info
// DO NOT PUSH YOU WiFi Info here update locally only
constexpr const char* WIFI_SSID = "Akaristudios";
constexpr const char* WIFI_PASSWORD = "StudioBros85!";

constexpr uint16_t kWebServerTaskStackBytes = 8192;

//Task Priorities to be updated
constexpr UBaseType_t kWebServerTaskPriority = 1;

constexpr uint16_t WS_PORT = 80;

}  // namespace Config
}  // namespace SleepSentinel