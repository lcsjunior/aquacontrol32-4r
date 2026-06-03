#ifndef DALLAS_TEMPERATURE_SENSOR_H
#define DALLAS_TEMPERATURE_SENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>
#include "temperature_sensor.h"

class DallasTemperatureSensor : public TemperatureSensor {
private:
  OneWire* oneWire_ = nullptr;
  DallasTemperature sensors_;
  unsigned long lastRequestMs_ = 0;
  float lastTemperatureC_ = DEVICE_DISCONNECTED_C;

public:
  ~DallasTemperatureSensor();
  void begin(const byte pin) override;
  void requestTemperatures() override;
  float temperatureC() override;
};

#endif // DALLAS_TEMPERATURE_SENSOR_H
