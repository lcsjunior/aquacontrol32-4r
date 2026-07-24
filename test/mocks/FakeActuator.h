#ifndef FAKE_ACTUATOR_H
#define FAKE_ACTUATOR_H

#include "Actuator.h"

class FakeActuator : public Actuator {
 public:
  void begin(byte) override {
  }

  void turnOn() override {
    isOn_ = true;
    turnOnCount_++;
  }

  void turnOff() override {
    isOn_ = false;
    turnOffCount_++;
  }

  void toggle() override {
    isOn_ ? turnOff() : turnOn();
  }

  bool isOn() const override {
    return isOn_;
  }

  int turnOnCount_ = 0;
  int turnOffCount_ = 0;

 private:
  bool isOn_ = false;
};

#endif  // FAKE_ACTUATOR_H
