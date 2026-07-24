#ifndef FAKE_TEMP_SENSOR_H
#define FAKE_TEMP_SENSOR_H

#include "TemperatureSensor.h"

class FakeTempSensor : public TemperatureSensor {
 public:
  void begin(const byte) override {
  }
  void requestTemperatures() override {
  }

  float temperatureC() override {
    return value_;
  }

  void temperatureC(float value) {
    value_ = value;
  }

 private:
  float value_ = 25.0f;
};

#endif  // FAKE_TEMP_SENSOR_H
