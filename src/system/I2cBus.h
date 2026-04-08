#pragma once

#include "freertos/FreeRTOS.h"

bool initI2cBus();
bool lockI2cBus(TickType_t timeout = portMAX_DELAY);
void unlockI2cBus();
