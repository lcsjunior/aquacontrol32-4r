#pragma once

#include "OneWire.h"

#define DEVICE_DISCONNECTED_C -127.0f

class DallasTemperature {
 public:
  DallasTemperature() = default;

  void setOneWire(OneWire*) {
  }
  void begin() {
  }
  void requestTemperatures() {
  }

  float getTempCByIndex(uint8_t) {
    return stubTemperatureC_;
  }

  static void stubTemperatureC(float value) {
    stubTemperatureC_ = value;
  }

 private:
  static float stubTemperatureC_;
};

inline float DallasTemperature::stubTemperatureC_ = 25.0f;
