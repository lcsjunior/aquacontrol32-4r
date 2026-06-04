#ifndef WIFI_LIB_H
#define WIFI_LIB_H

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <ArduinoOTA.h>

#define WIFI_CONNECT_TIMEOUT_MS 60000UL
#define CONFIG_TZ_DELAY_MS 3000UL

uint8_t dBmToQuality(const int16_t dBm);

class WiFiLib {
private:
  char _ssid[32];
  char _pass[32];
  unsigned long _lastWiFiRetryConnectTime = 0;
  bool _shouldReboot = false;
  bool _isSTAEnabled = false;
  uint32_t _chipId = 0;
  uint8_t _apChannel = 0;
  uint8_t _channel = 0;

public:
  void initAP(const char *apPass);
  void initSTA(const char *ssid, const char *pass, const char *otaPass,
               const char *tz, const char *hostname);
  void loop();
  uint32_t getChipId();
  void reboot();
};

extern WiFiLib WIFI;

#endif // WIFI_LIB_H