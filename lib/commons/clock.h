#ifndef CLOCK_H
#define CLOCK_H

#include <stddef.h>

#define DATETIME_FORMAT "%Y-%m-%d %H:%M:%S"

class Clock {
public:
  virtual ~Clock() = default;
  virtual unsigned long millis() const = 0;
  virtual void formatLocalDateTime(char *buf, size_t len) const = 0;
};

#endif // CLOCK_H
