#include "Queues.h"

#include "sensors/RoomTempSensor.h"
#include "system/Config.h"

QueueHandle_t hrQueue = nullptr;
QueueHandle_t spo2Queue = nullptr;
QueueHandle_t bodyTempQueue = nullptr;
QueueHandle_t roomTempQueue = nullptr;
QueueHandle_t motionQueue = nullptr;
QueueHandle_t soundQueue = nullptr;
QueueHandle_t airQueue = nullptr;
QueueHandle_t roomClimateQueue = nullptr;

bool initQueues() {
  if (hrQueue != nullptr && spo2Queue != nullptr && bodyTempQueue != nullptr &&
      roomTempQueue != nullptr && motionQueue != nullptr &&
      soundQueue != nullptr && airQueue != nullptr &&
      roomClimateQueue != nullptr) {
    return true;
  }

  if (hrQueue == nullptr) {
    hrQueue = xQueueCreate(10, sizeof(int));
  }

  if (spo2Queue == nullptr) {
    spo2Queue = xQueueCreate(10, sizeof(int));
  }

  if (bodyTempQueue == nullptr) {
    bodyTempQueue = xQueueCreate(10, sizeof(float));
  }

  if (roomTempQueue == nullptr) {
    roomTempQueue = xQueueCreate(10, sizeof(float));
  }

  if (motionQueue == nullptr) {
    motionQueue = xQueueCreate(10, sizeof(bool));
  }

  if (soundQueue == nullptr) {
    soundQueue = xQueueCreate(10, sizeof(int));
  }

  if (airQueue == nullptr) {
    airQueue = xQueueCreate(10, sizeof(int));
  }

  if (roomClimateQueue == nullptr) {
  roomClimateQueue = xQueueCreate(
      SleepSentinel::Config::kRoomClimateQueueLength,
      sizeof(RoomClimateReading));
  }

  return hrQueue != nullptr && spo2Queue != nullptr &&
         bodyTempQueue != nullptr && roomTempQueue != nullptr &&
         motionQueue != nullptr && soundQueue != nullptr &&
         airQueue != nullptr && roomClimateQueue != nullptr;
}
