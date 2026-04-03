#pragma once

// FreeRTOS task
void CommunicationTask(void *pvParameters);

// Optional: initialization (WiFi + server setup)
void initWiFiServer();