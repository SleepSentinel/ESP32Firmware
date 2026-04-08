#include "MessageFormatter.h"

String MessageFormatter::toJson(const SystemState& state) {
    String json = "{";

    json += "\"heartRate\":" + String(state.heartRate) + ",";
    json += "\"spo2\":" + String(state.spo2) + ",";
    json += "\"bodyTemperature\":" + String(state.bodyTemperature, 2) + ",";
    json += "\"roomTemperature\":" + String(state.roomTemperature, 2) + ",";
    json += "\"roomHumidity\":" + String(state.roomHumidity, 2) + ",";
    json += "\"roomSensorOk\":" + String(state.roomSensorOk ? "true" : "false") + ",";
    json += "\"isMoving\":" + String(state.isMoving ? "true" : "false") + ",";
    json += "\"isCrying\":" + String(state.isCrying ? "true" : "false") + ",";
    json += "\"airQuality\":" + String(state.airQuality) + ",";

    // Alerts
    json += "\"alertHighHR\":" + String(state.alertHighHR ? "true" : "false") + ",";
    json += "\"alertLowHR\":" + String(state.alertLowHR ? "true" : "false") + ",";
    json += "\"alertHighBodyTemp\":" + String(state.alertHighBodyTemp ? "true" : "false") + ",";
    json += "\"alertLowBodyTemp\":" + String(state.alertLowBodyTemp ? "true" : "false") + ",";
    json += "\"alertHighRoomTemp\":" + String(state.alertHighRoomTemp ? "true" : "false") + ",";
    json += "\"alertLowRoomTemp\":" + String(state.alertLowRoomTemp ? "true" : "false");

    json += "}";

    return json;
}

// for testing purposes
String MessageFormatter::dummyJson() {
    SystemState dummy;

    dummy.heartRate = random(60, 120);
    dummy.spo2 = random(95, 100);
    dummy.bodyTemperature = 36.0 + (random(0, 100) / 100.0);
    dummy.roomTemperature = 22.0 + (random(0, 100) / 10.0);
    dummy.roomHumidity = 40.0 + (random(0, 100) / 10.0);
    dummy.roomSensorOk = true;
    dummy.isMoving = random(0, 2);
    dummy.isCrying = random(0, 2);
    dummy.airQuality = random(0, 500);

    return toJson(dummy);
}