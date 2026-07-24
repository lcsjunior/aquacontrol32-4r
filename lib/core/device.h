#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>

const char* getApName();
const char* intToStr(int value);
const char* floatToStr(float value);
void waitWifi(uint32_t timeoutMs);
void waitNtp(uint32_t timeoutMs);

#endif  // DEVICE_H
