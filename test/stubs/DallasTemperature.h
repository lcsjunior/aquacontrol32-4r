#pragma once

#include "OneWire.h"

#define DEVICE_DISCONNECTED_C -127.0f

class DallasTemperature {
public:
    DallasTemperature() = default;
    explicit DallasTemperature(OneWire*) {}

    void begin() {}
    void requestTemperatures() {}

    float getTempCByIndex(uint8_t) {
        return stubTempC_;
    }

    static void setStubTempC(float value) {
        stubTempC_ = value;
    }

private:
    static float stubTempC_;
};

inline float DallasTemperature::stubTempC_ = 25.0f;
