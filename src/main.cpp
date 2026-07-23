#include <Arduino.h>
#include <CronAlarms.h>
#include <NoDelay.h>
#include <arduino_clock.h>
#include <config.h>
#include <dallas_temperature_sensor.h>
#include <mqtt_client.h>
#include <relay.h>
#include <telnet_logger.h>
#include <thermostat.h>

#include "modules/cron_setup.h"
#include "modules/http_server.h"
#include "modules/ota_setup.h"
#include "modules/wifi_setup.h"

#define LED_PIN 25
#define K1_PIN 21
#define K2_PIN 19
#define K4_PIN 5
#define DS18B20_PIN 22

#define SERIAL_BAUD_RATE 115200

#define MQTT_PUB_INTERVAL_MS 60000UL

DallasTemperatureSensor temperatureSensor;
Relay heater;
Relay lamp;
Relay co2;
Thermostat thermostat(&heater, &ArduinoClock);

noDelay pubInterval(MQTT_PUB_INTERVAL_MS);
char payload[255];

void mqttPublish();

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  AppConfig.mount();
  AppConfig.load();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  temperatureSensor.begin(DS18B20_PIN);
  heater.begin(K1_PIN, "heater");
  lamp.begin(K2_PIN, "lamp");
  co2.begin(K4_PIN, "co2");
  thermostat.begin(
      AppConfig.thermostatSetpoint(), AppConfig.thermostatHysteresis(),
      AppConfig.thermostatLowerLimit(), AppConfig.thermostatUpperLimit());

  initWifi();
  TelnetLog.begin();
  initOta();

  MQTT.begin(AppConfig.mqttHost(), AppConfig.mqttPort(),
             AppConfig.mqttClientId(), AppConfig.mqttUser(),
             AppConfig.mqttPass(), AppConfig.mqttSubTopic(),
             AppConfig.mqttPubTopic());
  MQTT.connect();

  initHttpServer();
  initCron();
}

void loop() {
  temperatureSensor.requestTemperatures();
  thermostat.update(temperatureSensor.temperatureC());

  wifiManager.process();
  ArduinoOTA.handle();
  Cron.delay();

  MQTT.loop();
  mqttPublish();

  TelnetLog.loop();
}

void mqttPublish() {
  if (!pubInterval.update())
    return;
  snprintf(payload, sizeof(payload),
           "field1=%.1f&field3=%d&field5=%d&field6=%d&status=PUB %s",
           temperatureSensor.temperatureC(), heater.isOn(), lamp.isOn(),
           co2.isOn(), formatLocalDateTime());
  log_i("%s", payload);
  MQTT.publish(payload);
}
