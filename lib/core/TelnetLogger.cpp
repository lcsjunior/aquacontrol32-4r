#include "TelnetLogger.h"

#include <stdarg.h>
#include <stdio.h>

extern "C" int ets_printf(const char* fmt, ...);

TelnetLogger TelnetLog;

void TelnetLogger::begin() {
  server_.begin();
}

void TelnetLogger::loop() {
  if (server_.hasClient())
    client_ = server_.available();
}

size_t TelnetLogger::write(const uint8_t* data, size_t len) {
  if (!client_ || !client_.connected())
    return 0;
  return client_.write(data, len);
}

extern "C" int __wrap_log_printfv(const char* format, va_list args) {
  static constexpr size_t LOG_BUFFER_SIZE = 256;
  va_list argsCopy;
  va_copy(argsCopy, args);
  char logMessage[LOG_BUFFER_SIZE];
  int n = vsnprintf(logMessage, sizeof(logMessage), format, argsCopy);
  va_end(argsCopy);

  if (n <= 0)
    return n;

  size_t len = (size_t)n < sizeof(logMessage) ? (size_t)n : sizeof(logMessage) - 1;
  ets_printf("%s", logMessage);
  TelnetLog.write(reinterpret_cast<const uint8_t*>(logMessage), len);
  return n;
}