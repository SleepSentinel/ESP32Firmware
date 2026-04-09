#include "WebSocketServer.h"
#include "server/MessageFormatter.h"
#include "system/SystemState.h" 

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
                client->text("{\"type\":\"sleepsentinel.hello\",\"device\":\"sleepsentinel-esp32\",\"product\":\"sleepsentinel\"}");
                break;

            case WS_EVT_DISCONNECT:
                Serial.printf("Client disconnected: %u\n", client->id());
                break;

            case WS_EVT_DATA:
                handleWebSocketMessage(client, arg, data, len);
                break;

            default:
                break;
        }
    });
}

// Handle Incoming Messages from Client
void WebSocketServer::handleWebSocketMessage(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len) {
    if (client == nullptr || arg == nullptr) {
        return;
    }

    AwsFrameInfo *info = static_cast<AwsFrameInfo *>(arg);
    if (!info->final || info->index != 0 || info->len != len || info->opcode != WS_TEXT) {
        return;
    }

    String message;
    message.reserve(len);
    for (size_t i = 0; i < len; i++) {
        message += static_cast<char>(data[i]);
    }

    if (message.indexOf("\"type\":\"sleepsentinel.discovery\"") >= 0 &&
        message.indexOf("\"action\":\"identify\"") >= 0) {
        client->text("{\"type\":\"sleepsentinel.discovery.response\",\"device\":\"sleepsentinel-esp32\",\"product\":\"sleepsentinel\"}");
    }
}

// Broadcast Loop -> sends data to client
void WebSocketServer::run() {
    static unsigned long lastSend = 0;

    unsigned long now = millis();

    if (now - lastSend > 1000) {

        // Gets thread-safe snapshot
        SystemState state = getSystemStateSnapshot();

        // Convert to JSON
        String json = MessageFormatter::toJson(state);

        // Sends to all connected clients
        ws.textAll(json);

        lastSend = now;
    }

    // Clean disconnected clients
    ws.cleanupClients();
}
