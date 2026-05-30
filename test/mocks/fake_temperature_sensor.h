#pragma once

#include "temperature_sensor.h"

class FakeTemperatureSensor : public TemperatureSensor {
public:
  void begin(const byte) override {}
  void requestTemperatures() override {}
  float getTemperatureC() override { return value_; }
  void setTemperatureC(float value) { value_ = value; }

private:
  float value_ = 25.0f;
};
