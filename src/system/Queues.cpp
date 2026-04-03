#include "Queues.h"

QueueHandle_t hrQueue;
QueueHandle_t spo2Queue;
QueueHandle_t bodyTempQueue;
QueueHandle_t roomTempQueue;
QueueHandle_t motionQueue;
QueueHandle_t soundQueue;
QueueHandle_t airQueue;

void initQueues() {
  hrQueue = xQueueCreate(10, sizeof(int));
  spo2Queue = xQueueCreate(10, sizeof(int));
  bodyTempQueue = xQueueCreate(10, sizeof(float));
  roomTempQueue = xQueueCreate(10, sizeof(float));
  motionQueue = xQueueCreate(10, sizeof(bool));
  soundQueue = xQueueCreate(10, sizeof(int));
  airQueue = xQueueCreate(10, sizeof(int));
}