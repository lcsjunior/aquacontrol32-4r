#ifndef FAKE_CLOCK_H
#define FAKE_CLOCK_H

#include "Clock.h"

class FakeClock : public Clock {
 public:
  unsigned long millis() const override {
    return now_;
  }

  void advance(unsigned long ms) {
    now_ += ms;
  }

 private:
  unsigned long now_ = 0;
};

#endif  // FAKE_CLOCK_H
