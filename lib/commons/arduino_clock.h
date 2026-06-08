#ifndef ARDUINO_CLOCK_H
#define ARDUINO_CLOCK_H

#include <Arduino.h>
#include "clock.h"

class ArduinoClockImpl : public Clock {
 public:
  unsigned long millis() const override {
    return ::millis();
  }
  const char* formatLocalDateTime();

 private:
  char dateTimeBuf_[20];
};

extern ArduinoClockImpl ArduinoClock;

#endif  // ARDUINO_CLOCK_H
