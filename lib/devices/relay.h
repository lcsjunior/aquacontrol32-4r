#ifndef RELAY_H
#define RELAY_H

#include "actuator.h"

class Relay : public Actuator {
private:
  byte pin_;
  bool isOn_ = false;
  void write();

public:
  void begin(const byte pin) override;
  bool isOn() const override;
  void turnOn() override;
  void turnOff() override;
  void toggle();
};

#endif // RELAY_H
