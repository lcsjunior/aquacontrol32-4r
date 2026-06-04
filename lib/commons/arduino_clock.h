#ifndef ARDUINO_CLOCK_H
#define ARDUINO_CLOCK_H

#include <Arduino.h>
#include "clock.h"

void formatLocalDateTime(char *buf, size_t len);

class ArduinoClock : public Clock {
public:
  unsigned long millis() const override { return ::millis(); }
};

#endif // ARDUINO_CLOCK_H
