#include <Arduino.h>
#include <ArduinoClock.h>
#include <Clock.h>
#include <Config.h>
#include <CronAlarms.h>
#include <DallasTempSensor.h>
#include <MQTTClient.h>
#include <NoDelay.h>
#include <Relay.h>
#include <TelnetLogger.h>
#include <Thermostat.h>

#include "modules/cron_setup.h"
#include "modules/http_server.h"
#include "modules/wifi_setup.h"

#define SERIAL_BAUD_RATE 115200
#define K1_PIN 21
#define K2_PIN 19
#define K4_PIN 5
#define DS18B20_PIN 22

#define MQTT_PUB_INTERVAL_MS 60000UL

DallasTempSensor temperatureSensor;
Relay heater;
Relay lamp;
Relay co2;
Thermostat thermostat(&heater, &ArduinoClock);

noDelay pubInterval(MQTT_PUB_INTERVAL_MS);
char payload[255];

void publish();

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  AppConfig.mount();
  AppConfig.load();

  temperatureSensor.begin(DS18B20_PIN);
  heater.begin(K1_PIN, "heater");
  lamp.begin(K2_PIN, "lamp");
  co2.begin(K4_PIN, "co2");
  thermostat.begin(
      AppConfig.thermostatSetpoint(), AppConfig.thermostatHysteresis(),
      AppConfig.thermostatLowerLimit(), AppConfig.thermostatUpperLimit());

  initWifi();
  TelnetLog.begin();
  initHttpServer();
  initCron();

  MQTT.begin(AppConfig.mqttHost(), AppConfig.mqttPort(),
             AppConfig.mqttClientId(), AppConfig.mqttUser(),
             AppConfig.mqttPass(), AppConfig.mqttSubTopic(),
             AppConfig.mqttPubTopic());
  MQTT.connect();
}

void loop() {
  temperatureSensor.requestTemperatures();
  thermostat.update(temperatureSensor.temperatureC());

  handleWifi();
  Cron.delay();

  MQTT.loop();
  publish();

  TelnetLog.loop();
}

void publish() {
  if (!pubInterval.update())
    return;
  snprintf(payload, sizeof(payload),
           "field1=%.1f&field3=%d&field5=%d&field6=%d&status=PUB %s",
           temperatureSensor.temperatureC(), heater.isOn(), lamp.isOn(),
           co2.isOn(), formatDateTime());
  log_i("%s", payload);
  MQTT.publish(payload);
}
