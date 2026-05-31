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
  }
}

void Relay::turnOff() {
  if (isOn_) {
    isOn_ = false;
    write();
  }
}

void Relay::toggle() { isOn_ ? turnOff() : turnOn(); }
