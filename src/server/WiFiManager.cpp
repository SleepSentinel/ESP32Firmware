// Connects to WiFi
// Retries if failed
// Provides connection status

#include "WiFiManager.h"
#include "../system/Config.h"

void WiFiManager::begin() {
    WiFi.mode(WIFI_STA);  // Station mode (connect to router)
    connect();
}

// Retry loop limited to 20 tries, 10s maximum
void WiFiManager::connect() {
    WiFi.begin(SleepSentinel::Config::WIFI_SSID, SleepSentinel::Config::WIFI_PASSWORD);

    Serial.print("Connecting to WiFi");

    int retryCount = 0;

    while (WiFi.status() != WL_CONNECTED && retryCount < 20) {
        delay(500);
        Serial.print(".");
        retryCount++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFailed to connect.");
    }
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}
