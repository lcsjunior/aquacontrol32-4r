#pragma once

#include "temperature_sensor.h"

class FakeTemperatureSensor : public TemperatureSensor {
public:
  void begin(const byte) override {}
  void requestTemperatures() override {}
  float temperatureC() override { return value_; }
  void temperatureC(float value) { value_ = value; }

private:
  float value_ = 25.0f;
};
