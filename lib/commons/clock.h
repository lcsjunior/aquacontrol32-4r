#ifndef CLOCK_H
#define CLOCK_H

class Clock {
public:
  virtual ~Clock() = default;
  virtual unsigned long millis() const = 0;
};

#endif // CLOCK_H
