#ifndef TELNET_LOGGER_H
#define TELNET_LOGGER_H

#include <Arduino.h>
#include <WiFi.h>

class TelnetLogger {
 public:
  void begin();
  void loop();
  size_t write(const uint8_t* data, size_t len);

 private:
  static constexpr uint16_t TELNET_PORT = 23;

  WiFiServer server_{TELNET_PORT};
  WiFiClient client_;
};

extern TelnetLogger TelnetLog;

#endif  // TELNET_LOGGER_H
