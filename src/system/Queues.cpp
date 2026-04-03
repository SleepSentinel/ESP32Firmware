#include "Queues.h"

#include "sensors/RoomTempSensor.h"
#include "system/Config.h"

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
