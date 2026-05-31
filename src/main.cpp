#include <Arduino.h>
#include <PubSubClient.h>
#include <CronAlarms.h>
#include <NoDelay.h>
#include <WebServer.h>
#include <secrets.h>
#include <config.h>
#include <espx_wifi.h>
#include <relay.h>
#include <dallas_temperature_sensor.h>
#include <thermostat.h>

#define LED_PIN 25
#define K1_PIN 21
#define K2_PIN 19
#define K4_PIN 5
#define ONEWIRE_PIN 22

#define MQTT_CONN_TIMEOUT (MILLIS_PER_SECOND * 5)
#define MQTT_PUB_TIMEOUT MILLIS_PER_MINUTE

const char *ssid = WIFI_SSID;
const char *pass = WIFI_PASS;
const char *otaPass = OTA_PASS;
const char *apPass = AP_PASS;
const char *tz = SAO_PAULO_TZ;
const char *hostname = "smart-aquarium";

const char *mqttServer = "mqtt3.thingspeak.com";
const int mqttPort = 1883;
const char *mqttClientId = MQTT_CLIENT_ID;
const char *mqttUsername = MQTT_USERNAME;
const char *mqttPassword = MQTT_PASSWORD;
unsigned long wrChId = SMART_AQUARIUM_WR_CH_ID;

const char *cronstr_at_07_30 = "0 30 7 * * *";
const char *cronstr_at_08_00 = "0 0 8 * * *";
const char *cronstr_at_14_30 = "0 30 14 * * *";
const char *cronstr_at_15_00 = "0 0 15 * * *";

WebServer server(80);
WiFiClient espClient;
PubSubClient pubSubClient(mqttServer, mqttPort, espClient);

noDelay mqttConnTime(MQTT_CONN_TIMEOUT);
noDelay mqttPubTime(MQTT_PUB_TIMEOUT);

char topic[32];
char msg[255];

TemperatureSensor* temperatureSensor = new DallasTemperatureSensor();
Actuator* heater = new Relay();
Actuator* lamp = new Relay();
Actuator* co2Valve = new Relay();
Thermostat thermostat(heater);

void writeMsg();
boolean mqttConnect();
boolean mqttReconnect();
void mqttPub();
void mqttSub();
void initCrons();
void initWS();

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  temperatureSensor->begin(ONEWIRE_PIN);
  heater->begin(K1_PIN);
  lamp->begin(K2_PIN);
  co2Valve->begin(K4_PIN);

  mountFS();
  if (!loadConfigFile()) {
    Serial.println(F("Using default config"));
    config.setpoint = 24;
    config.hysteresis = 0.5;
    saveConfigFile();
  }

  thermostat.begin(config.setpoint, config.hysteresis, 0, 30);

  WiFi.mode(WIFI_AP_STA);
  Wifi.initAP(apPass);
  Wifi.initSTA(ssid, pass, otaPass, tz, hostname);

  initWS();
  initCrons();
  mqttConnect();
}

void loop() {
  Wifi.loop();
  server.handleClient();
  Cron.delay();

  temperatureSensor->requestTemperatures();

  thermostat.update(temperatureSensor->getTemperatureC());

  mqttReconnect();
  pubSubClient.loop();
  mqttPub();
}

void writeMsg() {
  char tbuf[64];
  getLocalTimeFmt(tbuf, sizeof(tbuf));
  snprintf_P(
      msg, sizeof(msg),
      PSTR("field1=%.1f&field3=%d&field5=%d&field6=%d&"
           "status=PUB %s RSSI %d dBm (%d pcent)"),
      temperatureSensor->getTemperatureC(), heater->isOn(),
      lamp->isOn(), co2Valve->isOn(), tbuf, WiFi.RSSI(),
      dBm2Quality(WiFi.RSSI()));
}

boolean mqttConnect() {
  boolean result =
      pubSubClient.connect(mqttClientId, mqttUsername, mqttPassword);
  mqttSub();
  return result;
}

boolean mqttReconnect() {
  if (mqttConnTime.update() && !pubSubClient.connected()) {
    Serial.print(F("Attempting MQTT connection..."));
    if (mqttConnect()) {
      Serial.println("connected");
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(pubSubClient.state());
      Serial.print(F(" try again in "));
      Serial.print(MQTT_CONN_TIMEOUT / 1000);
      Serial.println(F(" seconds"));
    }
  };
  return pubSubClient.connected();
}

void mqttSub() {
  snprintf_P(topic, sizeof(topic), PSTR("channels/%ld/subscribe"), wrChId);
  pubSubClient.subscribe(topic);
}

void mqttPub() {
  if (mqttPubTime.update()) {
    writeMsg();
    snprintf_P(topic, sizeof(topic), PSTR("channels/%ld/publish"), wrChId);
    Serial.print(topic);
    Serial.print(F(" - "));
    Serial.print(msg);
    Serial.print(F(" ("));
    Serial.print(strnlen_P(msg, sizeof(msg)));
    Serial.println(F(" bytes)"));
    pubSubClient.publish(topic, msg);
  }
}

void initCrons() {
  Cron.create((char *)cronstr_at_07_30, []() { co2Valve->turnOn(); }, false);
  Cron.create((char *)cronstr_at_08_00, []() { lamp->turnOn(); }, false);
  Cron.create((char *)cronstr_at_14_30, []() { co2Valve->turnOff(); }, false);
  Cron.create((char *)cronstr_at_15_00, []() { lamp->turnOff(); }, false);
}

void initWS() {
  server.on(F("/"), []() {
    server.send(200, FPSTR(TEXT_PLAIN), FPSTR("Hello from ESP!"));
  });

  server.on(F("/reboot"), HTTP_GET, []() {
    Wifi.reboot();
    server.send(200);
  });

  server.on(F("/msg"), HTTP_GET, []() {
    char buf[sizeof(msg) + 32];
    size_t len = strnlen_P(msg, sizeof(msg));
    snprintf_P(buf, sizeof(buf), PSTR("%s (%zd bytes)"), msg, len);
    server.send(200, FPSTR(TEXT_PLAIN), FPSTR(buf));
  });

  server.on(F("/lamp/toggle"), HTTP_GET, []() {
    lamp->toggle();
    server.send(200);
  });

  server.on(F("/co2/toggle"), HTTP_GET, []() {
    co2Valve->toggle();
    server.send(200);
  });

  server.onNotFound(
      []() { server.send(404, FPSTR(TEXT_PLAIN), FPSTR("Not found")); });

  server.begin();
  Serial.println(F("HTTP server started"));
}
