#include "SystemState.h"

SystemState systemState;
SemaphoreHandle_t stateMutex;

void initSystemState() { stateMutex = xSemaphoreCreateMutex(); }