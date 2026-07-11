#include "wifi_setup.h"

#include <config.h>
#include <thermostat.h>
#include <utilities.h>

#define WIFI_PORTAL_TIMEOUT_S 300
#define WIFI_CONNECT_TIMEOUT_MS 30000UL

constexpr int LAMP_ON_CRON_IDX = 0;
constexpr int LAMP_OFF_CRON_IDX = 1;
constexpr int CO2_ON_CRON_IDX = 2;
constexpr int CO2_OFF_CRON_IDX = 3;

constexpr const char* TIMEZONE = "<-03>3";
constexpr const char* NTP_SERVER = "pool.ntp.org";

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
WiFiManagerParameter* wmThermostatSetpoint;
WiFiManagerParameter* wmThermostatHysteresis;
WiFiManagerParameter* wmThermostatLowerLimit;
WiFiManagerParameter* wmThermostatUpperLimit;

static void onWifiManagerSaveParams() {
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

  float thermostatSetpoint = atof(wmThermostatSetpoint->getValue());
  float thermostatHysteresis = atof(wmThermostatHysteresis->getValue());
  float thermostatLowerLimit = atof(wmThermostatLowerLimit->getValue());
  float thermostatUpperLimit = atof(wmThermostatUpperLimit->getValue());
  if (validateThermostatConfig(thermostatSetpoint, thermostatHysteresis,
                               thermostatLowerLimit, thermostatUpperLimit)) {
    AppConfig.setThermostatSetpoint(thermostatSetpoint);
    AppConfig.setThermostatHysteresis(thermostatHysteresis);
    AppConfig.setThermostatLowerLimit(thermostatLowerLimit);
    AppConfig.setThermostatUpperLimit(thermostatUpperLimit);
  } else {
    log_w(
        "Invalid thermostat config rejected: setpoint=%.1f hysteresis=%.1f "
        "lower=%.1f upper=%.1f",
        thermostatSetpoint, thermostatHysteresis, thermostatLowerLimit,
        thermostatUpperLimit);
  }

  AppConfig.save();
}

void initWifi() {
  // clang-format off
  wmOtaPass      = new WiFiManagerParameter("ota_pass",       "OTA Password",        AppConfig.otaPass(),               16, "type=\"password\"");
  wmMqttHost     = new WiFiManagerParameter("mqtt_host",      "MQTT Broker Host",    AppConfig.mqttHost(),              64);
  wmMqttPort     = new WiFiManagerParameter("mqtt_port",      "MQTT Broker Port",    intToStr(AppConfig.mqttPort()),     6, "type=\"number\"");
  wmMqttUser     = new WiFiManagerParameter("mqtt_user",      "MQTT Username",       AppConfig.mqttUser(),     			32);
  wmMqttPass     = new WiFiManagerParameter("mqtt_pass",      "MQTT Password",       AppConfig.mqttPass(),     			32, "type=\"password\"");
  wmMqttClientId = new WiFiManagerParameter("mqtt_client_id", "MQTT Client ID",      AppConfig.mqttClientId(), 			32);
  wmMqttPubTopic = new WiFiManagerParameter("mqtt_pub_topic", "MQTT Publish Topic",  AppConfig.mqttPubTopic(), 			32);
  wmMqttSubTopic = new WiFiManagerParameter("mqtt_sub_topic", "MQTT Subscribe Topic",AppConfig.mqttSubTopic(), 			32);
  wmCronLampOn   = new WiFiManagerParameter("cron_lamp_on",   "Lamp ON cron",        AppConfig.cron(LAMP_ON_CRON_IDX),  32);
  wmCronLampOff  = new WiFiManagerParameter("cron_lamp_off",  "Lamp OFF cron",       AppConfig.cron(LAMP_OFF_CRON_IDX), 32);
  wmCronCo2On    = new WiFiManagerParameter("cron_co2_on",    "CO2 ON cron",         AppConfig.cron(CO2_ON_CRON_IDX),   32);
  wmCronCo2Off   = new WiFiManagerParameter("cron_co2_off",   "CO2 OFF cron",        AppConfig.cron(CO2_OFF_CRON_IDX),  32);
  wmThermostatSetpoint   = new WiFiManagerParameter("thermostat_setpoint",    "Thermostat Setpoint (C)",     floatToStr(AppConfig.thermostatSetpoint()),   8, "type=\"number\" step=\"0.5\"");
  wmThermostatHysteresis = new WiFiManagerParameter("thermostat_hysteresis",  "Thermostat Hysteresis (C)",   floatToStr(AppConfig.thermostatHysteresis()), 8, "type=\"number\" step=\"0.5\"");
  wmThermostatLowerLimit = new WiFiManagerParameter("thermostat_lower_limit", "Thermostat Lower Limit (C)",  floatToStr(AppConfig.thermostatLowerLimit()), 8, "type=\"number\" step=\"0.5\"");
  wmThermostatUpperLimit = new WiFiManagerParameter("thermostat_upper_limit", "Thermostat Upper Limit (C)",  floatToStr(AppConfig.thermostatUpperLimit()), 8, "type=\"number\" step=\"0.5\"");
  // clang-format on

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
  wifiManager.addParameter(wmThermostatSetpoint);
  wifiManager.addParameter(wmThermostatHysteresis);
  wifiManager.addParameter(wmThermostatLowerLimit);
  wifiManager.addParameter(wmThermostatUpperLimit);

  wifiManager.setConfigPortalBlocking(false);
  wifiManager.setConfigPortalTimeout(WIFI_PORTAL_TIMEOUT_S);
  wifiManager.setHostname(DEVICE_HOSTNAME);
  wifiManager.setSaveParamsCallback(onWifiManagerSaveParams);
  wifiManager.setWiFiAutoReconnect(true);
  wifiManager.autoConnect(getApName(), AP_PASSWORD);
  wifiManager.startWebPortal();
  waitWifi(WIFI_CONNECT_TIMEOUT_MS);

  configTzTime(TIMEZONE, NTP_SERVER);
}
