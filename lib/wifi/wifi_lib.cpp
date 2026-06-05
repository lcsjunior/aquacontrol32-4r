#include "wifi_lib.h"

#define WIFI_CONNECT_TIMEOUT_MS 60000UL
#define WIFI_RECONNECT_INTERVAL_MS 60000UL
#define NTP_SYNC_DELAY_MS 3000UL

constexpr const char* NTP_SERVER = "pool.ntp.org";

WiFiLib WIFI;

uint8_t dBmToQuality(const int16_t dBm) {
  return (dBm <= -100) ? 0
       : (dBm >= -50)  ? 100
       : 2 * (dBm + 100);
}

void WiFiLib::initAP(const char *apPass) {
  static const IPAddress apIP(192, 168, 4, 1);
  static const IPAddress subnet(255, 255, 255, 0);

  char apSsid[32];
  snprintf_P(apSsid, sizeof(apSsid), "ESPsoftAP-%06x", getChipId());

  bool apConfigOk = WiFi.softAPConfig(apIP, apIP, subnet);
  if (!apConfigOk) log_w("[WiFiLib] softAPConfig failed");

  bool apStartOk = WiFi.softAP(apSsid, apPass);
  if (!apStartOk) log_w("[WiFiLib] softAP start failed");

  log_i("[WiFiLib] AP IP Address: %s", WiFi.softAPIP().toString().c_str());
  log_i("[WiFiLib] SSID: %s", WiFi.softAPSSID().c_str());
  log_i("[WiFiLib] AP MAC Address: %s", WiFi.softAPmacAddress().c_str());
  log_i("[WiFiLib] Board MAC Address: %s", WiFi.macAddress().c_str());
  log_i("[WiFiLib] Channel: %d", (int)WiFi.channel());

  delay(1000);
}

void WiFiLib::initSTA(const char *ssid, const char *pass, const char *otaPass,
                        const char *tz, const char *hostname) {
  strncpy(ssid_, ssid, sizeof(ssid_) - 1);
  ssid_[sizeof(ssid_) - 1] = '\0';
  strncpy(pass_, pass, sizeof(pass_) - 1);
  pass_[sizeof(pass_) - 1] = '\0';

  WiFi.setHostname(hostname);

  WiFi.begin(ssid_, pass_);
  Serial.print(F("Connecting"));
  unsigned long wifiConnectStartMs = millis();
  while (!WiFi.isConnected() && millis() - wifiConnectStartMs <= WIFI_CONNECT_TIMEOUT_MS) {
    Serial.print(F("."));
    delay(300);
  }
  staEnabled_ = true;
  Serial.println();

  configTzTime(tz, NTP_SERVER);
  unsigned long ntpSyncStartMs = millis();
  while (millis() - ntpSyncStartMs <= NTP_SYNC_DELAY_MS) {
    Serial.print(F("."));
    delay(200);
  }
  Serial.println();

  ArduinoOTA.setHostname((const char *)hostname);
  ArduinoOTA.setPassword((const char *)otaPass);
  ArduinoOTA.begin();

  log_i("[WiFiLib] IP Address: %s", WiFi.localIP().toString().c_str());
  log_i("[WiFiLib] Hostname: %s", WiFi.getHostname());
  log_i("[WiFiLib] Board MAC Address: %s", WiFi.macAddress().c_str());
  log_i("[WiFiLib] Wi-Fi Channel: %d", (int)WiFi.channel());

  int16_t rssi = WiFi.RSSI();
  log_i("[WiFiLib] Signal Strength: %d dBm / %u%%", (int)rssi, (unsigned)dBmToQuality(rssi));

  delay(1000);
}

void WiFiLib::reconnect() {
  if (millis() - lastReconnectAttemptMs_ < WIFI_RECONNECT_INTERVAL_MS) return;
  lastReconnectAttemptMs_ = millis();
  log_w("[WiFiLib] Reconnecting to WiFi...");
  WiFi.disconnect();
  WiFi.begin(ssid_, pass_);
}

void WiFiLib::loop() {
  if (rebootRequested_) {
    log_w("[WiFiLib] Rebooting...");
    delay(100);
    ESP.restart();
    return;
  }
  if (!staEnabled_) return;
  if (!WiFi.isConnected()) reconnect();
  ArduinoOTA.handle();
}

uint32_t WiFiLib::getChipId() {
  if (chipId_ > 0) return chipId_;
  for (int i = 0; i < 17; i = i + 8) {
    chipId_ |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  return chipId_;
}

void WiFiLib::reboot() { rebootRequested_ = true; }
