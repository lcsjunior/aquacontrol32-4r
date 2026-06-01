#pragma once

#include <cstdint>
#include <cstdio>
#include <cmath>

using std::isnan;

using byte = uint8_t;

inline unsigned long stubMillis_ = 0;

inline void setStubMillis(unsigned long value) {
    stubMillis_ = value;
}

inline unsigned long millis() {
    return stubMillis_;
}

inline int digitalWriteCallCount_ = 0;

inline void pinMode(byte, byte) {}
inline void digitalWrite(byte, byte) { digitalWriteCallCount_++; }

struct SerialStub {
    void println(const char*) {}
    void println(int) {}
    void print(const char*) {}
    void print(int) {}
    void print(float) {}
};

inline SerialStub Serial;

#define F(x) (x)
#define PSTR(x) (x)
#define FPSTR(x) (x)

#define INPUT  0
#define OUTPUT 1
#define HIGH   1
#define LOW    0

inline int logECallCount_ = 0;

#define log_i(fmt, ...) ((void)0)
#define log_w(fmt, ...) ((void)0)
#define log_d(fmt, ...) ((void)0)
#define log_e(fmt, ...) (logECallCount_++)
