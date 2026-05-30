#pragma once

#include <cstdint>
#include <cstdio>

using byte = uint8_t;

inline unsigned long stubMillis_ = 0;

inline void setStubMillis(unsigned long value) {
    stubMillis_ = value;
}

inline unsigned long millis() {
    return stubMillis_;
}

inline void pinMode(byte, byte) {}
inline void digitalWrite(byte, byte) {}

struct SerialStub {
    void println(const char*) {}
    void println(int) {}
};

inline SerialStub Serial;

#define INPUT  0
#define OUTPUT 1
#define HIGH   1
#define LOW    0

#define log_i(fmt, ...) ((void)0)
#define log_e(fmt, ...) ((void)0)
