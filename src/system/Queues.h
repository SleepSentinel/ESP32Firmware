#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern QueueHandle_t hrQueue;
extern QueueHandle_t spo2Queue;
extern QueueHandle_t bodyTempQueue;
extern QueueHandle_t roomTempQueue;
extern QueueHandle_t motionQueue;
extern QueueHandle_t soundQueue;
extern QueueHandle_t airQueue;
extern QueueHandle_t roomClimateQueue;

bool initQueues();
