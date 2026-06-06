#include <Arduino.h>
#include <CronAlarms.h>
#include <NoDelay.h>
#include <WebServer.h>
#include <secrets.h>
#include <arduino_clock.h>
#include <dallas_temperature_sensor.h>
#include <fs_lib.h>
#include <mqtt_client.h>
#include <relay.h>
#include <thermostat.h>
#include <wifi_lib.h>

#define LED_PIN 25
#define K1_PIN 21
#define K2_PIN 19
#define K4_PIN 5
#define DS18B20_PIN 22

#define SERIAL_BAUD_RATE 115200

#define MQTT_PUB_INTERVAL_MS 60000UL

constexpr const char* CRON_CO2_ON = "0 30 7 * * *";
constexpr const char* CRON_LAMP_ON = "0 0 8 * * *";
constexpr const char* CRON_CO2_OFF = "0 30 14 * * *";
constexpr const char* CRON_LAMP_OFF = "0 0 15 * * *";

constexpr const char* TEXT_PLAIN = "text/plain";
constexpr const char* APPLICATION_JSON = "application/json";

constexpr const char* TIMEZONE = "<-03>3";
constexpr const char* DEVICE_HOSTNAME = "aquacontrol32";

constexpr const char* MQTT_SERVER = "mqtt3.thingspeak.com";
constexpr int MQTT_PORT = 1883;

constexpr const char* MQTT_PUB_TOPIC = "channels/2421172/publish";
constexpr const char* MQTT_SUB_TOPIC = "channels/2421172/subscribe";

noDelay pubInterval(MQTT_PUB_INTERVAL_MS);
char payload[255];

WebServer server(8080);
WiFiClient espClient;

TemperatureSensor* temperatureSensor = new DallasTemperatureSensor();
Actuator* heater = new Relay();
Actuator* lamp = new Relay();
Actuator* co2 = new Relay();
ArduinoClock arduinoClock;
Thermostat thermostat(heater, &arduinoClock);

void buildPayload();
void mqttPublish();
void initCrons();
void initWS();

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  mountLittleFS();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  temperatureSensor->begin(DS18B20_PIN);
  heater->begin(K1_PIN);
  lamp->begin(K2_PIN);
  co2->begin(K4_PIN);

  thermostat.begin(24, 0.5, 0, 30);

  WiFi.mode(WIFI_AP_STA);
  WIFI.initAP(AP_PASS);
  WIFI.initSTA(WIFI_SSID, WIFI_PASS, OTA_PASS, TIMEZONE, DEVICE_HOSTNAME);

  initWS();
  initCrons();

  MQTT.begin(espClient, MQTT_SERVER, MQTT_PORT, MQTT_CLIENT_ID, MQTT_USERNAME,
             MQTT_PASSWORD);
  MQTT.connect();
  MQTT.subscribe(MQTT_SUB_TOPIC);
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
  char dateTime[64];
  formatLocalDateTime(dateTime, sizeof(dateTime));
  snprintf_P(payload, sizeof(payload),
             PSTR("field1=%.1f&field3=%d&field5=%d&field6=%d&"
                  "status=PUB %s RSSI %d dBm (%d pct)"),
             temperatureSensor->temperatureC(), heater->isOn(), lamp->isOn(),
             co2->isOn(), dateTime, WiFi.RSSI(), dBmToQuality(WiFi.RSSI()));
}

void mqttPublish() {
  if (pubInterval.update()) {
    buildPayload();
    MQTT.publish(MQTT_PUB_TOPIC, payload);
  }
}

void initCrons() {
  Cron.create((char*)CRON_CO2_ON, []() { co2->turnOn(); }, false);
  Cron.create((char*)CRON_LAMP_ON, []() { lamp->turnOn(); }, false);
  Cron.create((char*)CRON_CO2_OFF, []() { co2->turnOff(); }, false);
  Cron.create((char*)CRON_LAMP_OFF, []() { lamp->turnOff(); }, false);
}

void initWS() {
  server.on(F("/"), []() {
    server.send(200, FPSTR(TEXT_PLAIN), FPSTR("Hello from ESP32!"));
  });

  server.on(F("/reboot"), HTTP_GET, []() {
    WIFI.reboot();
    server.send(204);
  });

  server.on(F("/lamp/toggle"), HTTP_GET, []() {
    lamp->toggle();
    server.send(204);
  });

  server.on(F("/co2/toggle"), HTTP_GET, []() {
    co2->toggle();
    server.send(204);
  });

  server.onNotFound(
      []() { server.send(404, FPSTR(TEXT_PLAIN), FPSTR("Not found")); });

  server.begin();
  log_i("[main] HTTP server started");
}
