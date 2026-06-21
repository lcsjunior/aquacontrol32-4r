#ifndef UTILITIES_H
#define UTILITIES_H

#include <Arduino.h>

uint8_t dBmToQuality(int16_t dBm);
uint32_t getChipId();
const char* getApName();
const char* intToStr(int value);
const char* floatToStr(float value);

#endif
