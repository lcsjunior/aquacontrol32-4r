#include "relay.h"

void Relay::write() { digitalWrite(pin_, isOn_ ? HIGH : LOW); }

void Relay::begin(const byte pin) {
  pin_ = pin;
  pinMode(pin_, OUTPUT);
  write();
}

bool Relay::isOn() const { return isOn_; }

void Relay::turnOn() {
  if (!isOn_) {
    isOn_ = true;
    write();
    log_i("[Relay] Pin %d on", pin_);
  }
}

void Relay::turnOff() {
  if (isOn_) {
    isOn_ = false;
    write();
    log_i("[Relay] Pin %d off", pin_);
  }
}

void Relay::toggle() { isOn_ ? turnOff() : turnOn(); }
