#include <telnet_logger.h>
#include <esp_log.h>

TelnetLogger TelnetLog;

void TelnetLogger::begin() {
  esp_log_set_vprintf(&TelnetLogger::vprintfHook);
  server_.begin();
}

void TelnetLogger::loop() {
  if (server_.hasClient()) {
    if (client_)
      client_.stop();
    client_ = server_.available();
  }
}

int TelnetLogger::vprintfHook(const char* fmt, va_list args) {
  int written = vprintf(fmt, args);
  if (!TelnetLog.client_ || !TelnetLog.client_.connected())
    return written;

  va_list argsCopy;
  va_copy(argsCopy, args);
  char buf[LOG_BUFFER_SIZE];
  int len = vsnprintf(buf, sizeof(buf), fmt, argsCopy);
  va_end(argsCopy);

  if (len <= 0)
    return written;
  size_t toWrite = (size_t)len < sizeof(buf) ? (size_t)len : sizeof(buf) - 1;
  TelnetLog.client_.write(reinterpret_cast<const uint8_t*>(buf), toWrite);
  return written;
}
