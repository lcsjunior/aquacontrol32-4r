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

constexpr int LAMP_ON_CRON_IDX = 0;
constexpr int LAMP_OFF_CRON_IDX = 1;
constexpr int CO2_ON_CRON_IDX = 2;
constexpr int CO2_OFF_CRON_IDX = 3;

constexpr const char* APPLICATION_JSON = "application/json";

constexpr const char* TIMEZONE = "<-03>3";
constexpr const char* NTP_SERVER = "pool.ntp.org";
constexpr const char* DEVICE_HOSTNAME = "aquacontrol32";

WiFiClient espClient;
WiFiManager wifiManager;

WiFiManagerParameter* wmOtaPass;
WiFiManagerParameter* wmMqttHost;
WiFiManagerParameter* wmMqttPort;
WiFiManagerParameter* wmMqttUser;
WiFiManagerParameter* wmMqttPass;
WiFiManagerParameter* wmMqttClientId;
WiFiManagerParameter* wmMqttPubTopic;
WiFiManagerParameter* wmMqttSubTopic;
WiFiManagerParameter* wmCronLampOn;
WiFiManagerParameter* wmCronLampOff;
WiFiManagerParameter* wmCronCo2On;
WiFiManagerParameter* wmCronCo2Off;

TemperatureSensor* temperatureSensor = new DallasTemperatureSensor();
Actuator* heater = new Relay();
Actuator* lamp = new Relay();
Actuator* co2 = new Relay();
ArduinoClock arduinoClock;
Thermostat thermostat(heater, &arduinoClock);

noDelay pubInterval(MQTT_PUB_INTERVAL_MS);
char payload[255];

void initIO();
void initFs();
void initWifi();
void initOta();
void initMqtt();
void initHttpServer();
void initCrons();
void buildPayload();
void mqttPublish();
void onWifiManagerSaveParams();

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
  temperatureSensor->requestTemperatures();
  thermostat.update(temperatureSensor->temperatureC());

  wifiManager.process();
  ArduinoOTA.handle();
  Cron.delay();

  MQTT.loop();
  mqttPublish();
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
  // clang-format off
  wmOtaPass      = new WiFiManagerParameter("ota_pass",       "OTA Password",        AppConfig.otaPass(),               16, "type=\"password\"");
  wmMqttHost     = new WiFiManagerParameter("mqtt_host",      "MQTT Broker Host",    AppConfig.mqttHost(),              64);
  wmMqttPort     = new WiFiManagerParameter("mqtt_port",      "MQTT Broker Port",    intToStr(AppConfig.mqttPort()),     6, "type=\"number\"");
  wmMqttUser     = new WiFiManagerParameter("mqtt_user",      "MQTT Username",       AppConfig.mqttUser(),     32);
  wmMqttPass     = new WiFiManagerParameter("mqtt_pass",      "MQTT Password",       AppConfig.mqttPass(),     32, "type=\"password\"");
  wmMqttClientId = new WiFiManagerParameter("mqtt_client_id", "MQTT Client ID",      AppConfig.mqttClientId(), 32);
  wmMqttPubTopic = new WiFiManagerParameter("mqtt_pub_topic", "MQTT Publish Topic",  AppConfig.mqttPubTopic(), 32);
  wmMqttSubTopic = new WiFiManagerParameter("mqtt_sub_topic", "MQTT Subscribe Topic",AppConfig.mqttSubTopic(), 32);
  wmCronLampOn   = new WiFiManagerParameter("cron_lamp_on",   "Lamp ON cron",        AppConfig.cron(LAMP_ON_CRON_IDX),  32);
  wmCronLampOff  = new WiFiManagerParameter("cron_lamp_off",  "Lamp OFF cron",       AppConfig.cron(LAMP_OFF_CRON_IDX), 32);
  wmCronCo2On    = new WiFiManagerParameter("cron_co2_on",    "CO2 ON cron",         AppConfig.cron(CO2_ON_CRON_IDX),   32);
  wmCronCo2Off   = new WiFiManagerParameter("cron_co2_off",   "CO2 OFF cron",        AppConfig.cron(CO2_OFF_CRON_IDX),  32);
  // clang-format on

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
  wifiManager.addParameter(wmMqttPubTopic);
  wifiManager.addParameter(wmMqttSubTopic);
  wifiManager.addParameter(wmCronLampOn);
  wifiManager.addParameter(wmCronLampOff);
  wifiManager.addParameter(wmCronCo2On);
  wifiManager.addParameter(wmCronCo2Off);

  wifiManager.autoConnect(getApName(), AP_PASSWORD);

  configTzTime(TIMEZONE, NTP_SERVER);
}

void initOta() {
  ArduinoOTA.setHostname(DEVICE_HOSTNAME);
  ArduinoOTA.setPassword(AppConfig.otaPass());

  ArduinoOTA.onStart([]() { log_i("OTA start"); });
  ArduinoOTA.onEnd([]() { log_i("OTA end"); });
  ArduinoOTA.onError([](ota_error_t error) { log_e("OTA error %u", error); });

  ArduinoOTA.begin();
}

void initMqtt() {
  MQTT.begin(espClient, AppConfig);
  MQTT.connect();
  MQTT.subscribe(AppConfig.mqttSubTopic());
}

void initHttpServer() {
  wifiManager.server->on("/health", HTTP_GET, []() {
    wifiManager.server->send(200, APPLICATION_JSON, "{\"status\":\"UP\"}");
  });

  wifiManager.server->on("/lamp/toggle", HTTP_GET, []() {
    lamp->toggle();
    wifiManager.server->send(
        200, APPLICATION_JSON,
        lamp->isOn() ? "{\"active\":true}" : "{\"active\":false}");
  });

  wifiManager.server->on("/co2/toggle", HTTP_GET, []() {
    co2->toggle();
    wifiManager.server->send(
        200, APPLICATION_JSON,
        co2->isOn() ? "{\"active\":true}" : "{\"active\":false}");
  });
}

void initCrons() {
  if (WiFi.status() != WL_CONNECTED)
    return;
  Cron.create((char*)AppConfig.cron(LAMP_ON_CRON_IDX), []() { lamp->turnOn(); },
              false);
  Cron.create((char*)AppConfig.cron(LAMP_OFF_CRON_IDX),
              []() { lamp->turnOff(); }, false);
  Cron.create((char*)AppConfig.cron(CO2_ON_CRON_IDX), []() { co2->turnOn(); },
              false);
  Cron.create((char*)AppConfig.cron(CO2_OFF_CRON_IDX), []() { co2->turnOff(); },
              false);
}

void buildPayload() {
  char dateTimeBuf[64];
  formatLocalDateTime(dateTimeBuf, sizeof(dateTimeBuf));
  snprintf(payload, sizeof(payload),
           "field1=%.1f&field3=%d&field5=%d&field6=%d&"
           "status=PUB %s RSSI %d dBm (%d pct)",
           temperatureSensor->temperatureC(), heater->isOn(), lamp->isOn(),
           co2->isOn(), dateTimeBuf, WiFi.RSSI(), dBmToQuality(WiFi.RSSI()));
}

void mqttPublish() {
  if (!pubInterval.update())
    return;
  buildPayload();
  MQTT.publish(AppConfig.mqttPubTopic(), payload);
}

void onWifiManagerSaveParams() {
  AppConfig.setOtaPass(wmOtaPass->getValue());
  AppConfig.setMqttHost(wmMqttHost->getValue());
  AppConfig.setMqttPort((uint16_t)atoi(wmMqttPort->getValue()));
  AppConfig.setMqttUser(wmMqttUser->getValue());
  AppConfig.setMqttPass(wmMqttPass->getValue());
  AppConfig.setMqttClientId(wmMqttClientId->getValue());
  AppConfig.setMqttPubTopic(wmMqttPubTopic->getValue());
  AppConfig.setMqttSubTopic(wmMqttSubTopic->getValue());
  AppConfig.setCron(LAMP_ON_CRON_IDX, wmCronLampOn->getValue());
  AppConfig.setCron(LAMP_OFF_CRON_IDX, wmCronLampOff->getValue());
  AppConfig.setCron(CO2_ON_CRON_IDX, wmCronCo2On->getValue());
  AppConfig.setCron(CO2_OFF_CRON_IDX, wmCronCo2Off->getValue());
  AppConfig.save();
}