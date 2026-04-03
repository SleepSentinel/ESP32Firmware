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
  if (roomClimateQueue != nullptr) {
    return true;
  }

  roomClimateQueue = xQueueCreate(
      SleepSentinel::Config::kRoomClimateQueueLength,
      sizeof(RoomClimateReading));

  return roomClimateQueue != nullptr;
}
