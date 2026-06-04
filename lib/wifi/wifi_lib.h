#ifndef WIFI_LIB_H
#define WIFI_LIB_H

#include <Arduino.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <sys/time.h>
#else
#include <WiFi.h>
#include <esp_wifi.h>
#endif
#include <ArduinoOTA.h>
#include <LittleFS.h>
#include "../commons/clock.h"

#define SERIAL_BAUD_RATE 115200
#define FORMAT_LITTLEFS_IF_FAILED true
#define MILLIS_PER_SECOND 1000UL
#define MILLIS_PER_MINUTE (MILLIS_PER_SECOND * 60)
#define WIFI_CONNECT_TIMEOUT (MILLIS_PER_SECOND * 60)
#define CONFIG_TZ_DELAY (MILLIS_PER_SECOND * 3)
#define NTP_SERVER "pool.ntp.org"
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define SAO_PAULO_TZ "<-03>3"

static const char TEXT_PLAIN[] PROGMEM = "text/plain";
static const char TEXT_HTML[] PROGMEM = "text/html";
static const char APPLICATION_JSON[] PROGMEM = "application/json";

bool mountFS();
time_t now();
time_t uptime();
void getLocalTimeFmt(char *buf, size_t len);
void printLocalTime();
void printMAC(const uint8_t *mac_addr);
int strToMAC(const char *mac, uint8_t *values);
uint8_t dBmToQuality(const int16_t dBm);

class WiFiLib {
private:
  char _ssid[32];
  char _pass[32];
  const char *_ntpServer = NTP_SERVER;
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
  uint8_t getAPChannel() const;
  uint8_t getChannel() const;
  void reboot();
};
extern WiFiLib WIFI;

#endif // WIFI_LIB_H