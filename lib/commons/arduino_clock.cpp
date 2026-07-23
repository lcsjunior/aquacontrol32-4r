#include "arduino_clock.h"
#include <time.h>

#define DATETIME_FORMAT "%Y-%m-%d %H:%M:%S"

constexpr int NTP_SYNC_MIN_YEAR = 2024;

ArduinoClockImpl ArduinoClock;

const char* formatLocalDateTime() {
  static char buf[20];
  time_t now = time(nullptr);
  strftime(buf, sizeof(buf), DATETIME_FORMAT, localtime(&now));
  return buf;
}

bool isClockSynced() {
  return isClockSynced(time(nullptr));
}

bool isClockSynced(time_t now) {
  struct tm timeInfo;
  gmtime_r(&now, &timeInfo);
  if ((timeInfo.tm_year + 1900) < NTP_SYNC_MIN_YEAR) {
    log_w("Clock not synced (year=%d)", timeInfo.tm_year + 1900);
    return false;
  }
  return true;
}
