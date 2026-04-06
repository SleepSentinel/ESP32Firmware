// Connects to WiFi
// Retries if failed
// Provides connection status

#pragma once

#include <WiFi.h>

class WiFiManager {
    public:
        void begin();
        bool isConnected();

    private:
        void connect();
};