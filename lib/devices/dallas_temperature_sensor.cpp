#include "dallas_temperature_sensor.h"

#define TEMPERATURE_POLL_INTERVAL_MS 1000UL

void DallasTemperatureSensor::begin(const byte pin) {
  oneWire_.begin(pin);
  sensors_.setOneWire(&oneWire_);
  sensors_.begin();
  lastRequestMs_ = millis() - TEMPERATURE_POLL_INTERVAL_MS;
}

void DallasTemperatureSensor::requestTemperatures() {
  if (millis() - lastRequestMs_ < TEMPERATURE_POLL_INTERVAL_MS)
    return;
  sensors_.requestTemperatures();
  lastTemperatureC_ = sensors_.getTempCByIndex(0);
  if (lastTemperatureC_ == DEVICE_DISCONNECTED_C)
    log_e("[DallasTemperatureSensor] Could not read temperature");
  lastRequestMs_ = millis();
}

float DallasTemperatureSensor::temperatureC() {
  return lastTemperatureC_;
}
