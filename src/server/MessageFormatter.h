#pragma once

#include <Arduino.h>
#include "system/SystemState.h"

class MessageFormatter {
public:
    static String toJson(const SystemState& state);
    static String dummyJson(); // for testing only
};