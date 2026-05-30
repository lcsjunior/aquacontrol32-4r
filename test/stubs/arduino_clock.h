#pragma once

#include <Arduino.h>
#include "clock.h"

class ArduinoClockImpl : public Clock {
public:
  unsigned long millis() const override { return ::millis(); }
};

inline ArduinoClockImpl ArduinoClock;
