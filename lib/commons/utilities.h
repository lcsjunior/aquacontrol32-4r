#ifndef UTILITIES_H
#define UTILITIES_H

#include <Arduino.h>
#include <WiFi.h>

uint32_t getChipId();
const char* getApName();
const char* intToStr(int value);
const char* floatToStr(float value);
bool waitWifi(uint32_t timeoutMs);

#endif
