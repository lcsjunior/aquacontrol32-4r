#ifndef TELNET_LOGGER_H
#define TELNET_LOGGER_H

#include <Arduino.h>
#include <WiFi.h>

class TelnetLogger {
 public:
  void begin();
  void loop();

 private:
  static constexpr uint16_t TELNET_PORT = 23;
  static constexpr size_t LOG_BUFFER_SIZE = 256;

  WiFiServer server_{TELNET_PORT};
  WiFiClient client_;

  static int vprintfHook(const char* fmt, va_list args);
};

extern TelnetLogger TelnetLog;

#endif  // TELNET_LOGGER_H
