#pragma once

#include <ESPAsyncWebServer.h>

class WebSocketServer {
public:
    WebSocketServer(uint16_t port);

    void begin(); //starts server
    void run(); 

private:
    AsyncWebServer server;
    AsyncWebSocket ws;

    void setupWebSocket();
    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);

    String generateDummyData(); // temporary will be replace with SystemState Data
};

