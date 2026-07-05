#include <ota_client.h>

#include <Arduino.h>
#include <ArduinoOTA.h>

OTAClient OTA;

void OTAClient::begin(const char* hostname, const char* password) {
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.setPassword(password);

  ArduinoOTA.onStart([]() { log_i("OTA start"); });
  ArduinoOTA.onEnd([]() { log_i("OTA end"); });
  ArduinoOTA.onError([](ota_error_t error) { log_e("OTA error %u", error); });

  ArduinoOTA.begin();
}

void OTAClient::handle() {
  ArduinoOTA.handle();
}
