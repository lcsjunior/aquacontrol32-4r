#include "http_server.h"
#include "wifi_setup.h"

#include <arduino_clock.h>
#include <heating_state.h>
#include <relay.h>
#include <thermostat.h>

constexpr const char* APPLICATION_JSON = "application/json";

extern Relay lamp;
extern Relay co2;
extern Thermostat thermostat;

static bool requireAuth() {
  if (wifiManager.server->authenticate(WWW_USERNAME, WWW_PASSWORD))
    return true;
  wifiManager.server->requestAuthentication();
  return false;
}

void initHttpServer() {
  wifiManager.server->on("/health", HTTP_GET, []() {
    char healthBuf[64];
    snprintf(healthBuf, sizeof(healthBuf),
             "{\"status\":\"UP\",\"datetime\":\"%s\"}", formatLocalDateTime());
    wifiManager.server->send(200, APPLICATION_JSON, healthBuf);
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

  wifiManager.server->on("/heater/on", HTTP_GET, []() {
    if (!requireAuth())
      return;
    thermostat.forceTransition(&HeatingState);
    wifiManager.server->send(204);
  });
}
