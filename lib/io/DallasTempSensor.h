#ifndef DALLAS_TEMP_SENSOR_H
#define DALLAS_TEMP_SENSOR_H

#include <DallasTemperature.h>
#include <OneWire.h>
#include "TemperatureSensor.h"

class DallasTempSensor : public TemperatureSensor {
 public:
  void begin(const byte pin) override;
  void requestTemperatures() override;
  float temperatureC() override;

 private:
  OneWire oneWire_;
  DallasTemperature sensors_;
  unsigned long lastRequestMs_ = 0;
  float lastTemperatureC_ = DEVICE_DISCONNECTED_C;
};

#endif  // DALLAS_TEMP_SENSOR_H
