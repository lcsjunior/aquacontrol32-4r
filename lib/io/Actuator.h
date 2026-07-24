#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <Arduino.h>

class Actuator {
 public:
  virtual ~Actuator() = default;
  virtual void begin(byte pin) = 0;
  virtual void turnOn() = 0;
  virtual void turnOff() = 0;
  virtual void toggle() = 0;
  virtual bool isOn() const = 0;
};

#endif  // ACTUATOR_H
