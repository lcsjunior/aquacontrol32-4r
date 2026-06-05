#include "wifi_lib.h"

constexpr const char* NTP_SERVER = "pool.ntp.org";

WiFiLib WIFI;
IPAddress apIP(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

uint8_t dBmToQuality(const int16_t dBm) {
  if (dBm <= -100)
    return 0;
  else if (dBm >= -50)
    return 100;
  return 2 * (dBm + 100);
}

void WiFiLib::initAP(const char *apPass) {
  delay(1000);

  char apSsid[32];
  sprintf_P(apSsid, "ESPsoftAP-%06x", getChipId());

  bool apConfigOk = WiFi.softAPConfig(apIP, apIP, subnet);
  if (!apConfigOk) log_w("[WiFiLib] softAPConfig failed");

  bool apStartOk = WiFi.softAP(apSsid, apPass);
  if (!apStartOk) log_w("[WiFiLib] softAP start failed");

  log_i("[WiFiLib] AP IP Address: %s", WiFi.softAPIP().toString().c_str());
  log_i("[WiFiLib] SSID: %s", WiFi.softAPSSID().c_str());
  log_i("[WiFiLib] AP MAC Address: %s", WiFi.softAPmacAddress().c_str());
  log_i("[WiFiLib] Board MAC Address: %s", WiFi.macAddress().c_str());
  log_i("[WiFiLib] Channel: %d", (int)WiFi.channel());
  _apChannel = WiFi.channel();

  delay(1000);
}

void WiFiLib::initSTA(const char *ssid, const char *pass, const char *otaPass,
                        const char *tz, const char *hostname) {
  delay(1000);

  strcpy_P(_ssid, ssid);
  strcpy_P(_pass, pass);

  WiFi.setHostname(hostname);

  WiFi.begin(_ssid, _pass);
  unsigned long currentMillis = millis();
  while (!WiFi.isConnected() &&
         (millis() - currentMillis) <= WIFI_CONNECT_TIMEOUT_MS) {
    delay(300);
  }
  _isSTAEnabled = true;

  configTzTime(tz, NTP_SERVER);
  currentMillis = millis();
  while ((millis() - currentMillis) <= CONFIG_TZ_DELAY_MS) {
    delay(300);
  }
  delay(1000);

  ArduinoOTA.setHostname((const char *)hostname);
  ArduinoOTA.setPassword((const char *)otaPass);
  ArduinoOTA.begin();

  log_i("[WiFiLib] IP Address: %s", WiFi.localIP().toString().c_str());
  log_i("[WiFiLib] Hostname: %s", WiFi.getHostname());
  log_i("[WiFiLib] Board MAC Address: %s", WiFi.macAddress().c_str());
  log_i("[WiFiLib] Wi-Fi Channel: %d", (int)WiFi.channel());
  _channel = WiFi.channel();

  int16_t rssi = WiFi.RSSI();
  log_i("[WiFiLib] Signal Strength: %d dBm / %u%%", (int)rssi, (unsigned)dBmToQuality(rssi));

  delay(1000);
}

void WiFiLib::loop() {
  if (_shouldReboot) {
    log_i("[WiFiLib] Rebooting...");
    delay(100);
    ESP.restart();
  }
  if (_isSTAEnabled) {
    if (!WiFi.isConnected() &&
        (millis() - _lastWiFiRetryConnectTime) >= WIFI_CONNECT_TIMEOUT_MS) {
      _lastWiFiRetryConnectTime = millis();
      log_w("[WiFiLib] Reconnecting to WiFi...");
      WiFi.disconnect();
      WiFi.begin(_ssid, _pass);
    }
    ArduinoOTA.handle();
  }
}

uint32_t WiFiLib::getChipId() {
  if (_chipId > 0) return _chipId;
  for (int i = 0; i < 17; i = i + 8) {
    _chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  return _chipId;
}

void WiFiLib::reboot() { _shouldReboot = true; }
