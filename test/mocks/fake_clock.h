#pragma once

#include "clock.h"

class FakeClock : public Clock {
public:
  unsigned long millis() const override { return now_; }
  void advance(unsigned long ms) { now_ += ms; }

private:
  unsigned long now_ = 0;
};
