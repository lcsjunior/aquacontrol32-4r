#pragma once

#include "clock.h"

struct tm;

extern "C" {
  typedef long time_t;
  time_t time(time_t *) noexcept;
  struct tm *localtime(const time_t *) noexcept;
  size_t strftime(char *, size_t, const char *, const struct tm *) noexcept;
}

class ArduinoClockImpl : public Clock {
public:
  unsigned long millis() const override { return 0; }
  void formatLocalDateTime(char *buf, size_t len) const override {
    time_t now = time(nullptr);
    struct tm *timeinfo = localtime(&now);
    strftime(buf, len, DATETIME_FORMAT, timeinfo);
  }
};

inline ArduinoClockImpl ArduinoClock;
