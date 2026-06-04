#include "arduino_clock.h"
#include <time.h>

#define DATETIME_FORMAT "%Y-%m-%d %H:%M:%S"

void formatLocalDateTime(char *buf, size_t len) {
  time_t now = time(nullptr);
  struct tm *timeinfo = localtime(&now);
  strftime(buf, len, DATETIME_FORMAT, timeinfo);
}
