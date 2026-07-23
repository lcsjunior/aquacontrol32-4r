#include "http_server.h"
#include "wifi_setup.h"

#include <arduino_clock.h>
#include <dallas_temperature_sensor.h>
#include <relay.h>

constexpr const char* APPLICATION_JSON = "application/json";

extern DallasTemperatureSensor temperatureSensor;
extern Relay heater;
extern Relay lamp;
extern Relay co2;

static bool requireAuth() {
  if (wifiManager.server->authenticate(WWW_USERNAME, WWW_PASSWORD))
    return true;
  wifiManager.server->requestAuthentication();
  return false;
}

void initHttpServer() {
  wifiManager.server->on("/health", HTTP_GET, []() {
    char buf[192];
    snprintf(buf, sizeof(buf),
             "{\"status\":\"UP\",\"datetime\":\"%s\",\"temperatureC\":%.1f,"
             "\"relays\":{\"heater\":%s,\"lamp\":%s,\"co2\":%s}}",
             formatLocalDateTime(), temperatureSensor.temperatureC(),
             heater.activeText(), lamp.activeText(), co2.activeText());
    wifiManager.server->send(200, APPLICATION_JSON, buf);
  });

  wifiManager.server->on("/lamp/toggle", HTTP_GET, []() {
    if (!requireAuth())
      return;
    lamp.toggle();
    wifiManager.server->send(
        200, APPLICATION_JSON,
        lamp.isOn() ? "{\"active\":true}" : "{\"active\":false}");
  });

  wifiManager.server->on("/co2/toggle", HTTP_GET, []() {
    if (!requireAuth())
      return;
    co2.toggle();
    char buf[24];
    snprintf(buf, sizeof(buf), "{\"active\":%s}", co2.activeText());
    wifiManager.server->send(200, APPLICATION_JSON, buf);
  });
}
