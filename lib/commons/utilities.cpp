#include "utilities.h"

uint32_t getChipId() {
  uint32_t chipId = 0;
  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  return chipId;
}

uint8_t dBmToQuality(const int16_t dBm) {
  return (dBm <= -100) ? 0 : (dBm >= -50) ? 100 : 2 * (dBm + 100);
}
