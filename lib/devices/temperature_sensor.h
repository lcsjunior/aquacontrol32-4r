#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <Arduino.h>

class TemperatureSensor {
public:
  virtual ~TemperatureSensor() = default;
  virtual void begin(const byte pin) = 0;
  virtual void requestTemperatures() = 0;
  virtual float getTemperatureC() = 0;
};

#endif // TEMPERATURE_SENSOR_H
