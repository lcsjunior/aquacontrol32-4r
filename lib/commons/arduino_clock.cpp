#include "arduino_clock.h"
#include <time.h>

#define DATETIME_FORMAT "%Y-%m-%d %H:%M:%S"

ArduinoClockImpl ArduinoClock;

const char* ArduinoClockImpl::formatLocalDateTime() {
  time_t now = time(nullptr);
  strftime(dateTimeBuf_, sizeof(dateTimeBuf_), DATETIME_FORMAT,
           localtime(&now));
  return dateTimeBuf_;
}
