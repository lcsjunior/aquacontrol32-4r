#include "utilities.h"

// clang-format off
uint8_t dBmToQuality(const int16_t dBm) {
  return (dBm <= -100) ? 0
       : (dBm >= -50)  ? 100
                       : 2 * (dBm + 100);
}
// clang-format on

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
