#include "DallasTempSensor.h"

#define TEMPERATURE_POLL_INTERVAL_MS 1000UL

void DallasTempSensor::begin(const byte pin) {
  oneWire_.begin(pin);
  sensors_.setOneWire(&oneWire_);
  sensors_.begin();
  lastRequestMs_ = millis() - TEMPERATURE_POLL_INTERVAL_MS;
}

void DallasTempSensor::requestTemperatures() {
  if (millis() - lastRequestMs_ < TEMPERATURE_POLL_INTERVAL_MS)
    return;
  sensors_.requestTemperatures();
  lastTemperatureC_ = sensors_.getTempCByIndex(0);
  if (lastTemperatureC_ == DEVICE_DISCONNECTED_C)
    log_e("Could not read temperature");
  lastRequestMs_ = millis();
}

float DallasTempSensor::temperatureC() {
  return lastTemperatureC_;
}
