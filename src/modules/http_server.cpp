#include "http_server.h"
#include "wifi_setup.h"

#include <Clock.h>
#include <DallasTempSensor.h>
#include <Relay.h>

constexpr const char* APPLICATION_JSON = "application/json";

extern DallasTempSensor temperatureSensor;
extern Relay heater;
extern Relay lamp;
extern Relay co2;

static bool requireAuth() {
  if (wifiManager.server->authenticate(WWW_USERNAME, WWW_PASS))
    return true;
  wifiManager.server->requestAuthentication();
  return false;
}

void initHttpServer() {
  wifiManager.server->on("/health", HTTP_GET, []() {
    char payload[192];
    snprintf(payload, sizeof(payload),
             "{\"status\":\"UP\",\"datetime\":\"%s\",\"ds18b20\":%.2f,"
             "\"relays\":{\"heater\":%s,\"lamp\":%s,\"co2\":%s}}",
             formatDateTime(), temperatureSensor.temperatureC(),
             heater.activeText(), lamp.activeText(), co2.activeText());
    wifiManager.server->send(200, APPLICATION_JSON, payload);
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
    wifiManager.server->send(
        200, APPLICATION_JSON,
        co2.isOn() ? "{\"active\":true}" : "{\"active\":false}");
  });
}