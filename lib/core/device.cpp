#include "device.h"

#include <WiFi.h>

#include "Clock.h"

#define LOADING_DOT_INTERVAL_MS 250UL

static uint32_t getChipId() {
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
  static char text[12];
  snprintf(text, sizeof(text), "%d", value);
  return text;
}

const char* floatToStr(float value) {
  static char text[12];
  snprintf(text, sizeof(text), "%.1f", value);
  return text;
}

void waitWifi(uint32_t timeoutMs) {
  Serial.print("Waiting for WiFi connection...");

  const uint32_t startMs = millis();
  while (!WiFi.isConnected() && (millis() - startMs) <= timeoutMs) {
    Serial.print(".");
    delay(LOADING_DOT_INTERVAL_MS);
  }
  Serial.println(WiFi.isConnected() ? "connected" : "disconnected");
}

void waitNtp(uint32_t timeoutMs) {
  Serial.print("Waiting for NTP sync...");

  const uint32_t startMs = millis();
  while (!isTimeSet() && (millis() - startMs) <= timeoutMs) {
    Serial.print(".");
    delay(LOADING_DOT_INTERVAL_MS);
  }
  Serial.printf(isTimeSet() ? "synced (%s)\n" : "not synced\n",
                formatDateTime());
}
