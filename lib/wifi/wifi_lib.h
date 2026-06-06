#ifndef WIFI_LIB_H
#define WIFI_LIB_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <ArduinoOTA.h>

uint8_t dBmToQuality(const int16_t dBm);

class WiFiLib {
 public:
  void initAP(const char* apPass);
  void initSTA(const char* ssid, const char* pass, const char* otaPass,
               const char* tz, const char* hostname);
  void loop();
  uint32_t getChipId();
  void reboot();

 private:
  char ssid_[32];
  char pass_[32];
  unsigned long lastReconnectAttemptMs_ = 0;
  bool rebootRequested_ = false;
  bool staEnabled_ = false;
  uint32_t chipId_ = 0;

  void reconnect();
};

extern WiFiLib WIFI;

#endif  // WIFI_LIB_H