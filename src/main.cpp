#include <Arduino.h>
#include <ArduinoOTA.h>
#include <CronAlarms.h>
#include <NoDelay.h>
#include <WiFiManager.h>
#include <arduino_clock.h>
#include <config.h>
#include <dallas_temperature_sensor.h>
#include <mqtt_client.h>
#include <relay.h>
#include <thermostat.h>
#include <utilities.h>

#define LED_PIN 25
#define K1_PIN 21
#define K2_PIN 19
#define K4_PIN 5
#define DS18B20_PIN 22

#define SERIAL_BAUD_RATE 115200
#define MQTT_PUB_INTERVAL_MS 60000UL
#define WIFI_PORTAL_TIMEOUT_S 180

constexpr const char* CRON_CO2_ON = "0 30 7 * * *";
constexpr const char* CRON_LAMP_ON = "0 0 8 * * *";
constexpr const char* CRON_CO2_OFF = "0 30 14 * * *";
constexpr const char* CRON_LAMP_OFF = "0 0 15 * * *";

constexpr const char* APPLICATION_JSON = "application/json";

constexpr const char* TIMEZONE = "<-03>3";
constexpr const char* NTP_SERVER = "pool.ntp.org";
constexpr const char* DEVICE_HOSTNAME = "aquacontrol32";

constexpr const char* MQTT_PUB_TOPIC = "channels/2421172/publish";
constexpr const char* MQTT_SUB_TOPIC = "channels/2421172/subscribe";

WiFiClient espClient;
WiFiManager wifiManager;

WiFiManagerParameter* wmOtaPass;
WiFiManagerParameter* wmMqttHost;
WiFiManagerParameter* wmMqttPort;
WiFiManagerParameter* wmMqttUser;
WiFiManagerParameter* wmMqttPass;
WiFiManagerParameter* wmMqttClientId;

TemperatureSensor* temperatureSensor = new DallasTemperatureSensor();
Actuator* heater = new Relay();
Actuator* lamp = new Relay();
Actuator* co2 = new Relay();
ArduinoClock arduinoClock;
Thermostat thermostat(heater, &arduinoClock);

noDelay pubInterval(MQTT_PUB_INTERVAL_MS);
char payload[255];

void buildPayload();
void mqttPublish();
void onWifiManagerSaveParams();
void initIO();
void initFs();
void initWifi();
void initOta();
void initMqtt();
void initHttpServer();
void initCrons();

void setup() {
  initIO();
  initFs();
  initWifi();
  initOta();
  initMqtt();
  initHttpServer();
  initCrons();
}

void loop() {
  wifiManager.process();
  ArduinoOTA.handle();
  Cron.delay();

  temperatureSensor->requestTemperatures();
  thermostat.update(temperatureSensor->temperatureC());

  MQTT.loop();
  mqttPublish();
}

void buildPayload() {
  char dateTimeBuf[64];
  formatLocalDateTime(dateTimeBuf, sizeof(dateTimeBuf));
  snprintf_P(payload, sizeof(payload),
             PSTR("field1=%.1f&field3=%d&field5=%d&field6=%d&"
                  "status=PUB %s RSSI %d dBm (%d pct)"),
             temperatureSensor->temperatureC(), heater->isOn(), lamp->isOn(),
             co2->isOn(), dateTimeBuf, WiFi.RSSI(), dBmToQuality(WiFi.RSSI()));
}

void mqttPublish() {
  if (!pubInterval.update())
    return;
  buildPayload();
  MQTT.publish(MQTT_PUB_TOPIC, payload);
}

void onWifiManagerSaveParams() {
  AppConfig.setOtaPass(wmOtaPass->getValue());
  AppConfig.setMqttHost(wmMqttHost->getValue());
  AppConfig.setMqttPort((uint16_t)atoi(wmMqttPort->getValue()));
  AppConfig.setMqttUser(wmMqttUser->getValue());
  AppConfig.setMqttPass(wmMqttPass->getValue());
  AppConfig.setMqttClientId(wmMqttClientId->getValue());
  AppConfig.save();
}

void initIO() {
  Serial.begin(SERIAL_BAUD_RATE);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  temperatureSensor->begin(DS18B20_PIN);
  heater->begin(K1_PIN);
  lamp->begin(K2_PIN);
  co2->begin(K4_PIN);

  thermostat.begin(24, 0.5, 0, 30);
}

void initFs() {
  AppConfig.mount();
  AppConfig.load();
}

void initWifi() {
  char portBuf[7];
  snprintf(portBuf, sizeof(portBuf), "%u", AppConfig.mqttPort());

  wmOtaPass = new WiFiManagerParameter(
      "ota_pass", "OTA Password", AppConfig.otaPass(), 32, "type=\"password\"");
  wmMqttHost = new WiFiManagerParameter("mqtt_host", "MQTT Broker Host",
                                        AppConfig.mqttHost(), 64);
  wmMqttPort = new WiFiManagerParameter("mqtt_port", "MQTT Broker Port",
                                        portBuf, 6, "type=\"number\"");
  wmMqttUser = new WiFiManagerParameter("mqtt_user", "MQTT Username",
                                        AppConfig.mqttUser(), 48);
  wmMqttPass =
      new WiFiManagerParameter("mqtt_pass", "MQTT Password",
                               AppConfig.mqttPass(), 48, "type=\"password\"");
  wmMqttClientId = new WiFiManagerParameter("mqtt_client_id", "MQTT Client ID",
                                            AppConfig.mqttClientId(), 48);

  wifiManager.setConfigPortalBlocking(false);
  wifiManager.setConfigPortalTimeout(WIFI_PORTAL_TIMEOUT_S);
  wifiManager.setHostname(DEVICE_HOSTNAME);
  wifiManager.setSaveParamsCallback(onWifiManagerSaveParams);

  wifiManager.addParameter(wmOtaPass);
  wifiManager.addParameter(wmMqttHost);
  wifiManager.addParameter(wmMqttPort);
  wifiManager.addParameter(wmMqttUser);
  wifiManager.addParameter(wmMqttPass);
  wifiManager.addParameter(wmMqttClientId);

  wifiManager.autoConnect(getApName());
  wifiManager.startWebPortal();

  configTzTime(TIMEZONE, NTP_SERVER);
}

void initOta() {
  ArduinoOTA.setHostname(DEVICE_HOSTNAME);
  ArduinoOTA.setPassword(AppConfig.otaPass());

  ArduinoOTA.onStart([]() { log_i("[main] OTA start"); });
  ArduinoOTA.onEnd([]() { log_i("[main] OTA end"); });
  ArduinoOTA.onError(
      [](ota_error_t error) { log_e("[main] OTA error %u", error); });

  ArduinoOTA.begin();
}

void initMqtt() {
  MQTT.begin(espClient, AppConfig.mqttHost(), (int)AppConfig.mqttPort(),
             AppConfig.mqttClientId(), AppConfig.mqttUser(),
             AppConfig.mqttPass());
  MQTT.connect();
  MQTT.subscribe(MQTT_SUB_TOPIC);
}

void initHttpServer() {
  wifiManager.server->on(F("/health"), HTTP_GET, []() {
    wifiManager.server->send(200, FPSTR(APPLICATION_JSON),
                             F("{\"status\":\"UP\"}"));
  });

  wifiManager.server->on(F("/lamp/toggle"), HTTP_GET, []() {
    lamp->toggle();
    wifiManager.server->send(204);
  });

  wifiManager.server->on(F("/co2/toggle"), HTTP_GET, []() {
    co2->toggle();
    wifiManager.server->send(204);
  });
}

void initCrons() {
  Cron.create((char*)CRON_CO2_ON, []() { co2->turnOn(); }, false);
  Cron.create((char*)CRON_LAMP_ON, []() { lamp->turnOn(); }, false);
  Cron.create((char*)CRON_CO2_OFF, []() { co2->turnOff(); }, false);
  Cron.create((char*)CRON_LAMP_OFF, []() { lamp->turnOff(); }, false);
}
