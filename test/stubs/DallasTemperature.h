#pragma once

#include "OneWire.h"

class DallasTemperature {
public:
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
