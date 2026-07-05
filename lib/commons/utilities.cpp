#include "utilities.h"

#define LOADING_DOT_INTERVAL_MS 250UL

uint32_t getChipId() {
  uint32_t chipId = 0;
  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  return chipId;
}

const char* getApName() {
  static char name[32];
  snprintf(name, sizeof(name), "ESPsoftAP-%lu", (unsigned long)getChipId());
  return name;
}

const char* intToStr(int value) {
  static char buf[12];
  snprintf(buf, sizeof(buf), "%d", value);
  return buf;
}

const char* floatToStr(float value) {
  static char buf[12];
  snprintf(buf, sizeof(buf), "%.1f", value);
  return buf;
}

bool waitWifi(uint32_t timeoutMs) {
  Serial.println(F("Waiting for WiFi connection..."));
  const uint32_t startMs = millis();
  while (!WiFi.isConnected() && (millis() - startMs) <= timeoutMs) {
    Serial.print(F("."));
    delay(LOADING_DOT_INTERVAL_MS);
  }
  const bool connected = WiFi.isConnected();
  Serial.println(connected ? F("connected") : F("disconnected"));
  return connected;
}
