#include "WebSocketServer.h"
#include "server/MessageFormatter.h"

// Constructor
// React Native app connects to ws://<ESP32_IP>/ws
WebSocketServer::WebSocketServer(uint16_t port)
    : server(port), ws("/ws") {}

// Starts server
void WebSocketServer::begin() {
    setupWebSocket();

    server.addHandler(&ws);

    server.begin();

    Serial.println("WebSocket server started");
}

// WebSocket Events
void WebSocketServer::setupWebSocket() {
    ws.onEvent([this](AsyncWebSocket *server,
                      AsyncWebSocketClient *client,
                      AwsEventType type,
                      void *arg,
                      uint8_t *data,
                      size_t len) {

        switch (type) {
            case WS_EVT_CONNECT:
                Serial.printf("Client connected: %u\n", client->id());
                break;

            case WS_EVT_DISCONNECT:
                Serial.printf("Client disconnected: %u\n", client->id());
                break;

            case WS_EVT_DATA:
                handleWebSocketMessage(arg, data, len);
                break;

            default:
                break;
        }
    });
}

// Handle Incoming Messages from Client
void WebSocketServer::handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    // For now, we ignore incoming messages
}

//Dummy Data to be replace by struct SystemState in src/system/SystemState.h
String WebSocketServer::generateDummyData() {
    return MessageFormatter::dummyJson();
}

//Broadcast Loop -> send data to client
void WebSocketServer::run() {
    static unsigned long lastSend = 0;

    unsigned long now = millis();

    if (now - lastSend > 1000) {
        ws.textAll(generateDummyData());
        lastSend = now;
    }

    ws.cleanupClients();
}
