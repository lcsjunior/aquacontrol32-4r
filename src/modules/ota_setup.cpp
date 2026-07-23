#include "ota_setup.h"

#include <Arduino.h>
#include <config.h>

void initOta() {
  ArduinoOTA.setHostname(DEVICE_HOSTNAME);
  ArduinoOTA.setPassword(AppConfig.otaPass());

  ArduinoOTA.onStart([]() { log_i("OTA start"); });
  ArduinoOTA.onEnd([]() { log_i("OTA end"); });
  ArduinoOTA.onError([](ota_error_t error) { log_e("OTA error %u", error); });

  ArduinoOTA.begin();
}
