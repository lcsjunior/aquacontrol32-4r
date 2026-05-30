#include "dallas_temperature_sensor.h"

#define TEMPERATURE_POLL_INTERVAL_MS 1000UL

DallasTemperatureSensor::~DallasTemperatureSensor() {
  delete oneWire_;
}

void DallasTemperatureSensor::begin(const byte pin) {
  delete oneWire_;
  oneWire_ = new OneWire(pin);
  sensors_ = DallasTemperature(oneWire_);
  sensors_.begin();
}

void DallasTemperatureSensor::requestTemperatures() {
  if ((millis() - lastRequestMs_) >= TEMPERATURE_POLL_INTERVAL_MS) {
    sensors_.requestTemperatures();
    lastRequestMs_ = millis();
  }
}

float DallasTemperatureSensor::getTemperatureC() {
  float cTemp = sensors_.getTempCByIndex(0);
  if (cTemp == DEVICE_DISCONNECTED_C) {
    log_e("[DallasTemperatureSensor] Could not read temperature");
  }
  return cTemp;
}
