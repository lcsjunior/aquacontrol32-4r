#include <Arduino.h>
#include <CronAlarms.h>
#include <LittleFS.h>
#include <NoDelay.h>
#include <WebServer.h>
#include <secrets.h>
#include <config.h>
#include <wifi_lib.h>
#include <arduino_clock.h>
#include <relay.h>
#include <dallas_temperature_sensor.h>
#include <thermostat.h>
#include <mqtt_client.h>

#define LED_PIN 25
#define K1_PIN 21
#define K2_PIN 19
#define K4_PIN 5
#define DS18B20_PIN 22

#define MQTT_PUB_INTERVAL_MS 60000UL

constexpr int SERIAL_BAUD_RATE = 115200;
constexpr const char* TEXT_PLAIN = "text/plain";
constexpr const char* APPLICATION_JSON = "application/json";

constexpr const char* ssid = WIFI_SSID;
constexpr const char* pass = WIFI_PASS;
constexpr const char* otaPass = OTA_PASS;
constexpr const char* apPass = AP_PASS;
constexpr const char* tz = "<-03>3";
constexpr const char* hostname = "smart-aquarium";

constexpr const char* mqttServer = "mqtt3.thingspeak.com";
constexpr int mqttPort = 1883;
constexpr const char* mqttClientId = MQTT_CLIENT_ID;
constexpr const char* mqttUsername = MQTT_USERNAME;
constexpr const char* mqttPassword = MQTT_PASSWORD;

constexpr const char* publishTopic  = "channels/2421172/publish";
constexpr const char* subscribeTopic = "channels/2421172/subscribe";

noDelay publishInterval(MQTT_PUB_INTERVAL_MS);
char payload[255];

constexpr const char* cronstr_at_07_30 = "0 30 7 * * *";
constexpr const char* cronstr_at_08_00 = "0 0 8 * * *";
constexpr const char* cronstr_at_14_30 = "0 30 14 * * *";
constexpr const char* cronstr_at_15_00 = "0 0 15 * * *";

WebServer server(80);
WiFiClient espClient;

TemperatureSensor* temperatureSensor = new DallasTemperatureSensor();
Actuator* heater = new Relay();
Actuator* lamp = new Relay();
Actuator* co2 = new Relay();
Thermostat thermostat(heater);

void buildPayload();
void mqttPublish();
void initCrons();
void initWS();

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  temperatureSensor->begin(DS18B20_PIN);
  heater->begin(K1_PIN);
  lamp->begin(K2_PIN);
  co2->begin(K4_PIN);

  if (!LittleFS.begin(true)) log_e("[main] Failed to mount LittleFS");
  if (!loadConfigFile()) {
    log_i("[main] Using default config");
    config.setpoint = 24;
    config.hysteresis = 0.5;
    saveConfigFile();
  }

  thermostat.begin(config.setpoint, config.hysteresis, 0, 30);

  WiFi.mode(WIFI_AP_STA);
  WIFI.initAP(apPass);
  WIFI.initSTA(ssid, pass, otaPass, tz, hostname);

  initWS();
  initCrons();

  MQTT.begin(espClient, mqttServer, mqttPort, mqttClientId, mqttUsername, mqttPassword);
  MQTT.connect();
  MQTT.subscribe(subscribeTopic);
}

void loop() {
  WIFI.loop();
  server.handleClient();
  Cron.delay();

  temperatureSensor->requestTemperatures();

  thermostat.update(temperatureSensor->temperatureC());

  MQTT.loop();
  mqttPublish();
}

void buildPayload() {
  char tbuf[64];
  formatLocalDateTime(tbuf, sizeof(tbuf));
  snprintf_P(
      payload, sizeof(payload),
      PSTR("field1=%.1f&field3=%d&field5=%d&field6=%d&"
           "status=PUB %s RSSI %d dBm (%d pcent)"),
      temperatureSensor->temperatureC(), heater->isOn(),
      lamp->isOn(), co2->isOn(), tbuf, WiFi.RSSI(),
      dBmToQuality(WiFi.RSSI()));
}

void mqttPublish() {
  if (publishInterval.update()) {
    buildPayload();
    MQTT.publish(publishTopic, payload);
  }
}

void initCrons() {
  Cron.create((char *)cronstr_at_07_30, []() { co2->turnOn(); }, false);
  Cron.create((char *)cronstr_at_08_00, []() { lamp->turnOn(); }, false);
  Cron.create((char *)cronstr_at_14_30, []() { co2->turnOff(); }, false);
  Cron.create((char *)cronstr_at_15_00, []() { lamp->turnOff(); }, false);
}

void initWS() {
  server.on(F("/"), []() {
    server.send(200, TEXT_PLAIN, "Hello from ESP!");
  });

  server.on(F("/reboot"), HTTP_GET, []() {
    WIFI.reboot();
    server.send(200);
  });

  server.on(F("/msg"), HTTP_GET, []() {
    char buf[sizeof(payload) + 32];
    size_t len = strnlen_P(payload, sizeof(payload));
    snprintf_P(buf, sizeof(buf), PSTR("%s (%zd bytes)"), payload, len);
    server.send(200, TEXT_PLAIN, buf);
  });

  server.on(F("/lamp/toggle"), HTTP_GET, []() {
    lamp->toggle();
    server.send(200);
  });

  server.on(F("/co2/toggle"), HTTP_GET, []() {
    co2->toggle();
    server.send(200);
  });

  server.onNotFound(
      []() { server.send(404, TEXT_PLAIN, "Not found"); });

  server.begin();
  log_i("[main] HTTP server started");
}
