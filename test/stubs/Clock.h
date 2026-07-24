#ifndef CLOCK_H
#define CLOCK_H

#include <time.h>

constexpr int MIN_VALID_YEAR = 2024;

class Clock {
 public:
  virtual ~Clock() = default;
  virtual unsigned long millis() const = 0;
};

inline const char* formatDateTime() {
  static char dateTime[20];
  time_t now = time(nullptr);
  strftime(dateTime, sizeof(dateTime), "%Y-%m-%d %H:%M:%S", localtime(&now));
  return dateTime;
}

inline bool isTimeSet() {
  const time_t now = time(nullptr);
  struct tm timeInfo;
  gmtime_r(&now, &timeInfo);
  return (timeInfo.tm_year + 1900) >= MIN_VALID_YEAR;
}

template <typename T>
void safeCron(T& obj, void (T::*action)()) {
  if (isTimeSet())
    (obj.*action)();
}

#endif  // CLOCK_H
