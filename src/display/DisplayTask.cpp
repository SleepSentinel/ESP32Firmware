#include "DisplayTask.h"

#include <Arduino.h>

#include "LcdDisplay.h"
#include "system/Config.h"
#include "system/SystemState.h"

void DisplayTask(void* pvParameters) {
  (void)pvParameters;

  LcdDisplay lcdDisplay;
  lcdDisplay.begin();
  lcdDisplay.showStartup();

  vTaskDelay(pdMS_TO_TICKS(SleepSentinel::Config::kStartupScreenDurationMs));

  TickType_t lastWakeTime = xTaskGetTickCount();

  while (true) {
    lcdDisplay.render(getSystemStateSnapshot());
    vTaskDelayUntil(
        &lastWakeTime,
        pdMS_TO_TICKS(SleepSentinel::Config::kDisplayUpdateIntervalMs));
  }
}
