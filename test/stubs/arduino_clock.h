#pragma once

#include <Arduino.h>
#include <time.h>
#include "clock.h"

constexpr int NTP_SYNC_MIN_YEAR = 2024;

class ArduinoClockImpl : public Clock {
 public:
  unsigned long millis() const override {
    return ::millis();
  }
};

inline ArduinoClockImpl ArduinoClock;

inline bool isClockSynced(time_t now) {
  struct tm timeInfo;
  gmtime_r(&now, &timeInfo);
  return (timeInfo.tm_year + 1900) >= NTP_SYNC_MIN_YEAR;
}