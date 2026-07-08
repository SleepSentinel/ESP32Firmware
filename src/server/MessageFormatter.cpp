#include "MessageFormatter.h"

namespace {

void appendBoolField(String& json, const char* key, bool value) {
    json += "\"";
    json += key;
    json += "\":";
    json += value ? "true" : "false";
}

void appendIntField(String& json, const char* key, uint32_t value) {
    json += "\"";
    json += key;
    json += "\":";
    json += String(value);
}

void appendFloatField(String& json, const char* key, float value) {
    json += "\"";
    json += key;
    json += "\":";
    json += String(value, 2);
}

void appendAlertField(String& json, const char* key, bool value, bool& first) {
    if (!first) {
        json += ",";
    }

    appendBoolField(json, key, value);
    first = false;
}

void appendActiveAlert(String& json, const char* key, bool active, bool& first) {
    if (!active) {
        return;
    }

    if (!first) {
        json += ",";
    }

    json += "\"";
    json += key;
    json += "\"";
    first = false;
}

}  // namespace

String MessageFormatter::toJson(const SystemState& state) {
    String json;
    json.reserve(512);
    json = "{";

    appendIntField(json, "heartRate", state.heartRate);
    json += ",";
    appendIntField(json, "spo2", state.spo2);
    json += ",";
    appendFloatField(json, "bodyTemperature", state.bodyTemperature);
    json += ",";
    appendFloatField(json, "roomTemperature", state.roomTemperature);
    json += ",";
    appendFloatField(json, "roomHumidity", state.roomHumidity);
    json += ",";
    appendBoolField(json, "roomSensorOk", state.roomSensorOk);
    json += ",";
    appendBoolField(json, "motionSensorOk", state.motionSensorOk);
    json += ",";
    appendFloatField(json, "motionAccelX", state.motionAccelX);
    json += ",";
    appendFloatField(json, "motionAccelY", state.motionAccelY);
    json += ",";
    appendFloatField(json, "motionAccelZ", state.motionAccelZ);
    json += ",";
    appendBoolField(json, "isMoving", state.isMoving);
    json += ",";
    appendBoolField(json, "isCrying", state.isCrying);
    json += ",";
    appendIntField(json, "airQuality", state.airQuality);
    json += ",";
    appendBoolField(json, "airQualitySensorOk", state.airQualitySensorOk);
    json += ",";
    appendIntField(json, "alertVersion", state.alertVersion);
    json += ",";
    appendIntField(json, "lastAlertChangeMs", state.lastAlertChangeMs);
    json += ",";

    // Legacy flat alert flags retained for backwards compatibility.
    appendBoolField(json, "alertHighHR", state.alertHighHR);
    json += ",";
    appendBoolField(json, "alertLowHR", state.alertLowHR);
    json += ",";
    appendBoolField(json, "alertLowSpO2", state.alertLowSpO2);
    json += ",";
    appendBoolField(json, "alertHighBodyTemp", state.alertHighBodyTemp);
    json += ",";
    appendBoolField(json, "alertLowBodyTemp", state.alertLowBodyTemp);
    json += ",";
    appendBoolField(json, "alertHighRoomTemp", state.alertHighRoomTemp);
    json += ",";
    appendBoolField(json, "alertLowRoomTemp", state.alertLowRoomTemp);
    json += ",";

    json += "\"alerts\":{";
    bool firstAlert = true;
    appendAlertField(json, "highHR", state.alertHighHR, firstAlert);
    appendAlertField(json, "lowHR", state.alertLowHR, firstAlert);
    appendAlertField(json, "lowSpO2", state.alertLowSpO2, firstAlert);
    appendAlertField(json, "highBodyTemp", state.alertHighBodyTemp,
                     firstAlert);
    appendAlertField(json, "lowBodyTemp", state.alertLowBodyTemp,
                     firstAlert);
    appendAlertField(json, "highRoomTemp", state.alertHighRoomTemp,
                     firstAlert);
    appendAlertField(json, "lowRoomTemp", state.alertLowRoomTemp,
                     firstAlert);
    json += "},";

    json += "\"activeAlerts\":[";
    bool firstActiveAlert = true;
    appendActiveAlert(json, "highHR", state.alertHighHR, firstActiveAlert);
    appendActiveAlert(json, "lowHR", state.alertLowHR, firstActiveAlert);
    appendActiveAlert(json, "lowSpO2", state.alertLowSpO2,
                      firstActiveAlert);
    appendActiveAlert(json, "highBodyTemp", state.alertHighBodyTemp,
                      firstActiveAlert);
    appendActiveAlert(json, "lowBodyTemp", state.alertLowBodyTemp,
                      firstActiveAlert);
    appendActiveAlert(json, "highRoomTemp", state.alertHighRoomTemp,
                      firstActiveAlert);
    appendActiveAlert(json, "lowRoomTemp", state.alertLowRoomTemp,
                      firstActiveAlert);
    json += "]";

    json += "}";

    return json;
}

// for testing purposes
String MessageFormatter::dummyJson() {
    SystemState dummy = {};

    dummy.heartRate = random(60, 120);
    dummy.spo2 = random(95, 100);
    dummy.bodyTemperature = 36.0 + (random(0, 100) / 100.0);
    dummy.roomTemperature = 22.0 + (random(0, 100) / 10.0);
    dummy.roomHumidity = 40.0 + (random(0, 100) / 10.0);
    dummy.roomSensorOk = true;
    dummy.motionSensorOk = true;
    dummy.motionAccelX = 0.0f;
    dummy.motionAccelY = 0.0f;
    dummy.motionAccelZ = 0.0f;
    dummy.isMoving = random(0, 2);
    dummy.isCrying = random(0, 2);
    dummy.airQuality = random(0, 500);
    dummy.airQualitySensorOk = true;

    return toJson(dummy);
}
