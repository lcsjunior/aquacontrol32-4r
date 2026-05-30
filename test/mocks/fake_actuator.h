#pragma once

#include "actuator.h"

class FakeActuator : public Actuator {
public:
  void turnOn() override {
    isOn_ = true;
    turnOnCount_++;
  }

  void turnOff() override {
    isOn_ = false;
    turnOffCount_++;
  }

  bool isOn() const override { return isOn_; }

  int turnOnCount_ = 0;
  int turnOffCount_ = 0;

private:
  bool isOn_ = false;
};
