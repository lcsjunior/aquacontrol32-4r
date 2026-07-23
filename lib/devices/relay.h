#ifndef RELAY_H
#define RELAY_H

#include "actuator.h"

class Relay : public Actuator {
 public:
  void begin(const byte pin) override;
  void begin(const byte pin, const char* label);
  bool isOn() const override;
  const char* activeText() const;
  void turnOn() override;
  void turnOff() override;
  void toggle();

 private:
  byte pin_;
  bool isOn_ = false;
  const char* label_ = nullptr;
  void write();
};

#endif  // RELAY_H
