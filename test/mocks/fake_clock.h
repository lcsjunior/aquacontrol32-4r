#pragma once

#include "clock.h"

class FakeClock : public Clock {
public:
  unsigned long millis() const override { return now_; }
  void formatLocalDateTime(char *buf, size_t len) const override { buf[0] = '\0'; }
  void advance(unsigned long ms) { now_ += ms; }

private:
  unsigned long now_ = 0;
};
