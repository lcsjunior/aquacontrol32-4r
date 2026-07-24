#include "wifi_setup.h"

#include <ArduinoOTA.h>
#include <Config.h>
#include <Thermostat.h>
#include <device.h>

#define WIFI_PORTAL_TIMEOUT_S 300
#define WIFI_CONNECT_TIMEOUT_MS 30000UL
#define NTP_SYNC_TIMEOUT_MS 5000UL

constexpr const char* TIMEZONE = "<-03>3";
constexpr const char* NTP_SERVER = "pool.ntp.org";

constexpr const char* NO_DEFAULT = "";

constexpr int OTA_PASS_LEN = 15;
constexpr int MQTT_HOST_LEN = 63;
constexpr int MQTT_PORT_LEN = 5;
constexpr int MQTT_USER_LEN = 31;
constexpr int MQTT_PASS_LEN = 31;
constexpr int MQTT_CLIENT_ID_LEN = 31;
constexpr int MQTT_TOPIC_LEN = 31;
constexpr int CRON_LEN = 31;
constexpr int THERMOSTAT_LEN = 7;

WiFiManager wifiManager;

// clang-format off
WiFiManagerParameter wmOtaPass("ota_pass",               "OTA Password",         NO_DEFAULT, OTA_PASS_LEN,      "type=\"password\"");
WiFiManagerParameter wmMqttHost("mqtt_host",             "MQTT Broker Host",     NO_DEFAULT, MQTT_HOST_LEN);
WiFiManagerParameter wmMqttPort("mqtt_port",             "MQTT Broker Port",     NO_DEFAULT, MQTT_PORT_LEN,     "type=\"number\"");
WiFiManagerParameter wmMqttUser("mqtt_user",             "MQTT Username",        NO_DEFAULT, MQTT_USER_LEN);
WiFiManagerParameter wmMqttPass("mqtt_pass",             "MQTT Password",        NO_DEFAULT, MQTT_PASS_LEN,     "type=\"password\"");
WiFiManagerParameter wmMqttClientId("mqtt_client_id",    "MQTT Client ID",       NO_DEFAULT, MQTT_CLIENT_ID_LEN);
WiFiManagerParameter wmMqttPubTopic("mqtt_pub_topic",    "MQTT Publish Topic",   NO_DEFAULT, MQTT_TOPIC_LEN);
WiFiManagerParameter wmMqttSubTopic("mqtt_sub_topic",    "MQTT Subscribe Topic", NO_DEFAULT, MQTT_TOPIC_LEN);
WiFiManagerParameter wmCronLampOn("cron_lamp_on",        "Lamp ON cron",         NO_DEFAULT, CRON_LEN);
WiFiManagerParameter wmCronLampOff("cron_lamp_off",      "Lamp OFF cron",        NO_DEFAULT, CRON_LEN);
WiFiManagerParameter wmCronCo2On("cron_co2_on",          "CO2 ON cron",          NO_DEFAULT, CRON_LEN);
WiFiManagerParameter wmCronCo2Off("cron_co2_off",        "CO2 OFF cron",         NO_DEFAULT, CRON_LEN);
WiFiManagerParameter wmThermostatSetpoint("thermostat_setpoint",     "Thermostat Setpoint (C)",    NO_DEFAULT, THERMOSTAT_LEN, "type=\"number\" step=\"0.5\"");
WiFiManagerParameter wmThermostatHysteresis("thermostat_hysteresis", "Thermostat Hysteresis (C)",  NO_DEFAULT, THERMOSTAT_LEN, "type=\"number\" step=\"0.5\"");
WiFiManagerParameter wmThermostatLowerLimit("thermostat_lower_limit","Thermostat Lower Limit (C)", NO_DEFAULT, THERMOSTAT_LEN, "type=\"number\" step=\"0.5\"");
WiFiManagerParameter wmThermostatUpperLimit("thermostat_upper_limit","Thermostat Upper Limit (C)", NO_DEFAULT, THERMOSTAT_LEN, "type=\"number\" step=\"0.5\"");
// clang-format on

static void onWifiManagerSaveParams() {
  AppConfig.setOtaPass(wmOtaPass.getValue());
  AppConfig.setMqttHost(wmMqttHost.getValue());
  AppConfig.setMqttPort((uint16_t)atoi(wmMqttPort.getValue()));
  AppConfig.setMqttUser(wmMqttUser.getValue());
  AppConfig.setMqttPass(wmMqttPass.getValue());
  AppConfig.setMqttClientId(wmMqttClientId.getValue());
  AppConfig.setMqttPubTopic(wmMqttPubTopic.getValue());
  AppConfig.setMqttSubTopic(wmMqttSubTopic.getValue());
  AppConfig.setCron(LAMP_ON_CRON_IDX, wmCronLampOn.getValue());
  AppConfig.setCron(LAMP_OFF_CRON_IDX, wmCronLampOff.getValue());
  AppConfig.setCron(CO2_ON_CRON_IDX, wmCronCo2On.getValue());
  AppConfig.setCron(CO2_OFF_CRON_IDX, wmCronCo2Off.getValue());

  const float setpoint = atof(wmThermostatSetpoint.getValue());
  const float hysteresis = atof(wmThermostatHysteresis.getValue());
  const float lowerLimit = atof(wmThermostatLowerLimit.getValue());
  const float upperLimit = atof(wmThermostatUpperLimit.getValue());
  if (validateThermostatConfig(setpoint, hysteresis, lowerLimit, upperLimit)) {
    AppConfig.setThermostatSetpoint(setpoint);
    AppConfig.setThermostatHysteresis(hysteresis);
    AppConfig.setThermostatLowerLimit(lowerLimit);
    AppConfig.setThermostatUpperLimit(upperLimit);
  } else {
    log_w(
        "Invalid thermostat config rejected: setpoint=%.1f hysteresis=%.1f "
        "lower=%.1f upper=%.1f",
        setpoint, hysteresis, lowerLimit, upperLimit);
  }

  AppConfig.save();
}

static void initPortal() {
  wmOtaPass.setValue(AppConfig.otaPass(), OTA_PASS_LEN);
  wmMqttHost.setValue(AppConfig.mqttHost(), MQTT_HOST_LEN);
  wmMqttPort.setValue(intToStr(AppConfig.mqttPort()), MQTT_PORT_LEN);
  wmMqttUser.setValue(AppConfig.mqttUser(), MQTT_USER_LEN);
  wmMqttPass.setValue(AppConfig.mqttPass(), MQTT_PASS_LEN);
  wmMqttClientId.setValue(AppConfig.mqttClientId(), MQTT_CLIENT_ID_LEN);
  wmMqttPubTopic.setValue(AppConfig.mqttPubTopic(), MQTT_TOPIC_LEN);
  wmMqttSubTopic.setValue(AppConfig.mqttSubTopic(), MQTT_TOPIC_LEN);
  wmCronLampOn.setValue(AppConfig.cron(LAMP_ON_CRON_IDX), CRON_LEN);
  wmCronLampOff.setValue(AppConfig.cron(LAMP_OFF_CRON_IDX), CRON_LEN);
  wmCronCo2On.setValue(AppConfig.cron(CO2_ON_CRON_IDX), CRON_LEN);
  wmCronCo2Off.setValue(AppConfig.cron(CO2_OFF_CRON_IDX), CRON_LEN);
  wmThermostatSetpoint.setValue(floatToStr(AppConfig.thermostatSetpoint()),
                                THERMOSTAT_LEN);
  wmThermostatHysteresis.setValue(floatToStr(AppConfig.thermostatHysteresis()),
                                  THERMOSTAT_LEN);
  wmThermostatLowerLimit.setValue(floatToStr(AppConfig.thermostatLowerLimit()),
                                  THERMOSTAT_LEN);
  wmThermostatUpperLimit.setValue(floatToStr(AppConfig.thermostatUpperLimit()),
                                  THERMOSTAT_LEN);

  wifiManager.addParameter(&wmOtaPass);
  wifiManager.addParameter(&wmMqttHost);
  wifiManager.addParameter(&wmMqttPort);
  wifiManager.addParameter(&wmMqttUser);
  wifiManager.addParameter(&wmMqttPass);
  wifiManager.addParameter(&wmMqttClientId);
  wifiManager.addParameter(&wmMqttPubTopic);
  wifiManager.addParameter(&wmMqttSubTopic);
  wifiManager.addParameter(&wmCronLampOn);
  wifiManager.addParameter(&wmCronLampOff);
  wifiManager.addParameter(&wmCronCo2On);
  wifiManager.addParameter(&wmCronCo2Off);
  wifiManager.addParameter(&wmThermostatSetpoint);
  wifiManager.addParameter(&wmThermostatHysteresis);
  wifiManager.addParameter(&wmThermostatLowerLimit);
  wifiManager.addParameter(&wmThermostatUpperLimit);

  wifiManager.setConfigPortalBlocking(false);
  wifiManager.setConfigPortalTimeout(WIFI_PORTAL_TIMEOUT_S);
  wifiManager.setHostname(DEVICE_HOSTNAME);
  wifiManager.setSaveParamsCallback(onWifiManagerSaveParams);
  wifiManager.setWiFiAutoReconnect(true);
  wifiManager.autoConnect(getApName());
  wifiManager.startWebPortal();
  waitWifi(WIFI_CONNECT_TIMEOUT_MS);
}

static void initNtp() {
  configTzTime(TIMEZONE, NTP_SERVER);
  waitNtp(NTP_SYNC_TIMEOUT_MS);
}

static void initOta() {
  ArduinoOTA.setHostname(DEVICE_HOSTNAME);
  ArduinoOTA.setPassword(AppConfig.otaPass());

  ArduinoOTA.onStart([]() { log_i("OTA start"); });
  ArduinoOTA.onEnd([]() { log_i("OTA end"); });
  ArduinoOTA.onError([](ota_error_t error) { log_e("OTA error %u", error); });

  ArduinoOTA.begin();
}

void initWifi() {
  initPortal();
  initNtp();
  initOta();
}

void handleWifi() {
  wifiManager.process();
  ArduinoOTA.handle();
}
