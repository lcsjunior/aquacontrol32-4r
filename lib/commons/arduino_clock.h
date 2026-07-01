#ifndef ARDUINO_CLOCK_H
#define ARDUINO_CLOCK_H

#include <Arduino.h>
#include <time.h>
#include "clock.h"

class ArduinoClockImpl : public Clock {
 public:
  unsigned long millis() const override {
    return ::millis();
  }
};

extern ArduinoClockImpl ArduinoClock;

const char* formatLocalDateTime();
bool isClockSynced(time_t now);

#endif  // ARDUINO_CLOCK_H
