#ifndef RELAY_H
#define RELAY_H

#include "actuator.h"

class Relay : public Actuator {
 public:
  void begin(const byte pin) override;
  bool isOn() const override;
  void turnOn() override;
  void turnOff() override;
  void toggle();

 private:
  byte pin_;
  bool isOn_ = false;
  void write();
};

#endif  // RELAY_H
