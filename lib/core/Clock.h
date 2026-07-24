#ifndef CLOCK_H
#define CLOCK_H

class Clock {
 public:
  virtual ~Clock() = default;
  virtual unsigned long millis() const = 0;
};

const char* formatDateTime();
bool isTimeSet();

template <typename T>
void safeCron(T& obj, void (T::*action)()) {
  if (isTimeSet())
    (obj.*action)();
}

#endif  // CLOCK_H
