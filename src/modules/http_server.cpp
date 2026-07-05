#include "http_server.h"
#include "wifi_setup.h"

#include <arduino_clock.h>
#include <relay.h>

constexpr const char* APPLICATION_JSON = "application/json";

extern Relay lamp;
extern Relay co2;

void initHttpServer() {
  wifiManager.server->on("/health", HTTP_GET, []() {
    char healthBuf[64];
    snprintf(healthBuf, sizeof(healthBuf),
             "{\"status\":\"UP\",\"datetime\":\"%s\"}", formatLocalDateTime());
    wifiManager.server->send(200, APPLICATION_JSON, healthBuf);
  });

  wifiManager.server->on("/lamp/toggle", HTTP_GET, []() {
    lamp.toggle();
    wifiManager.server->send(
        200, APPLICATION_JSON,
        lamp.isOn() ? "{\"active\":true}" : "{\"active\":false}");
  });

  wifiManager.server->on("/co2/toggle", HTTP_GET, []() {
    co2.toggle();
    wifiManager.server->send(
        200, APPLICATION_JSON,
        co2.isOn() ? "{\"active\":true}" : "{\"active\":false}");
  });
}
