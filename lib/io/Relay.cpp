#include "Relay.h"

void Relay::write() {
  digitalWrite(pin_, isOn_ ? HIGH : LOW);
}

void Relay::begin(const byte pin) {
  begin(pin, nullptr);
}

void Relay::begin(const byte pin, const char* label) {
  pin_ = pin;
  label_ = label;
  pinMode(pin_, OUTPUT);
  write();
}

bool Relay::isOn() const {
  return isOn_;
}

const char* Relay::activeText() const {
  return isOn_ ? "true" : "false";
}

void Relay::turnOn() {
  if (!isOn_) {
    isOn_ = true;
    write();
    label_ ? log_i("%s on", label_) : log_i("Pin %d on", pin_);
  }
}

void Relay::turnOff() {
  if (isOn_) {
    isOn_ = false;
    write();
    label_ ? log_i("%s off", label_) : log_i("Pin %d off", pin_);
  }
}

void Relay::toggle() {
  isOn_ ? turnOff() : turnOn();
}
