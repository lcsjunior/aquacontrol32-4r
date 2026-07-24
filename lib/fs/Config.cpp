#include "Config.h"

#include <LittleFS.h>

constexpr const char* CONFIG_PATH = "/config.json";

constexpr const char* DEFAULT_MQTT_HOST = "mqtt3.thingspeak.com";
constexpr uint16_t DEFAULT_MQTT_PORT = 1883;
constexpr float DEFAULT_THERMOSTAT_SETPOINT = 23.5f;
constexpr float DEFAULT_THERMOSTAT_HYSTERESIS = 0.5f;
constexpr float DEFAULT_THERMOSTAT_LOWER_LIMIT = 0.0f;
constexpr float DEFAULT_THERMOSTAT_UPPER_LIMIT = 28.0f;

Config AppConfig;

void Config::mount() {
  if (LittleFS.begin()) {
    log_i("LittleFS mounted");
    return;
  }

  log_w("Failed to mount LittleFS, formatting");

  if (!LittleFS.format() || !LittleFS.begin()) {
    log_e("Failed to format LittleFS");
    return;
  }

  log_i("LittleFS formatted and mounted");
}

void Config::load() {
  applyDefaults();

  JsonDocument doc;
  if (!readFile(doc))
    return;

  convertFromJson(doc);
}

void Config::save() {
  JsonDocument doc;
  convertToJson(doc);
  writeFile(doc);
}

bool Config::readFile(JsonDocument& doc) {
  File file = LittleFS.open(CONFIG_PATH, "r");
  if (!file) {
    log_w("No %s yet, keeping defaults", CONFIG_PATH);
    return false;
  }

  DeserializationError err = deserializeJson(doc, file);
  const size_t bytesRead = file.size();
  file.close();

  if (err) {
    log_e("Parse error: %s", err.c_str());
    return false;
  }

  log_i("Loaded %u bytes from %s", bytesRead, CONFIG_PATH);
  return true;
}

bool Config::writeFile(const JsonDocument& doc) {
  File file = LittleFS.open(CONFIG_PATH, "w");
  if (!file) {
    log_e("Failed to open %s for writing", CONFIG_PATH);
    return false;
  }

  const size_t bytesWritten = serializeJson(doc, file);
  file.close();

  if (bytesWritten == 0) {
    log_e("Failed to write to %s", CONFIG_PATH);
    return false;
  }

  log_i("Saved %u bytes to %s", bytesWritten, CONFIG_PATH);
  return true;
}

const char* Config::otaPass() const {
  return otaPass_;
}

const char* Config::mqttHost() const {
  return mqttHost_;
}

uint16_t Config::mqttPort() const {
  return mqttPort_;
}

const char* Config::mqttUser() const {
  return mqttUser_;
}

const char* Config::mqttPass() const {
  return mqttPass_;
}

const char* Config::mqttClientId() const {
  return mqttClientId_;
}

const char* Config::mqttPubTopic() const {
  return mqttPubTopic_;
}

const char* Config::mqttSubTopic() const {
  return mqttSubTopic_;
}

const char* Config::cron(int index) const {
  if (index < 0 || index >= cronLength)
    return "";
  return crons_[index];
}

float Config::thermostatSetpoint() const {
  return thermostatSetpoint_;
}

float Config::thermostatHysteresis() const {
  return thermostatHysteresis_;
}

float Config::thermostatLowerLimit() const {
  return thermostatLowerLimit_;
}

float Config::thermostatUpperLimit() const {
  return thermostatUpperLimit_;
}

void Config::setOtaPass(const char* value) {
  strlcpy(otaPass_, value, sizeof(otaPass_));
}

void Config::setMqttHost(const char* value) {
  strlcpy(mqttHost_, value, sizeof(mqttHost_));
}

void Config::setMqttPort(uint16_t value) {
  mqttPort_ = value;
}

void Config::setMqttUser(const char* value) {
  strlcpy(mqttUser_, value, sizeof(mqttUser_));
}

void Config::setMqttPass(const char* value) {
  strlcpy(mqttPass_, value, sizeof(mqttPass_));
}

void Config::setMqttClientId(const char* value) {
  strlcpy(mqttClientId_, value, sizeof(mqttClientId_));
}

void Config::setMqttPubTopic(const char* value) {
  strlcpy(mqttPubTopic_, value, sizeof(mqttPubTopic_));
}

void Config::setMqttSubTopic(const char* value) {
  strlcpy(mqttSubTopic_, value, sizeof(mqttSubTopic_));
}

void Config::setCron(int index, const char* value) {
  if (index < 0 || index >= cronLength) {
    return;
  }

  strlcpy(crons_[index], value, sizeof(crons_[index]));
}

void Config::setThermostatSetpoint(float value) {
  thermostatSetpoint_ = value;
}

void Config::setThermostatHysteresis(float value) {
  thermostatHysteresis_ = value;
}

void Config::setThermostatLowerLimit(float value) {
  thermostatLowerLimit_ = value;
}

void Config::setThermostatUpperLimit(float value) {
  thermostatUpperLimit_ = value;
}

void Config::applyDefaults() {
  setOtaPass(OTA_PASS);
  setMqttHost(DEFAULT_MQTT_HOST);
  setMqttPort(DEFAULT_MQTT_PORT);
  setMqttUser("");
  setMqttPass("");
  setMqttClientId("");
  setMqttPubTopic("");
  setMqttSubTopic("");
  setCron(LAMP_ON_CRON_IDX, "");
  setCron(LAMP_OFF_CRON_IDX, "");
  setCron(CO2_ON_CRON_IDX, "");
  setCron(CO2_OFF_CRON_IDX, "");
  setThermostatSetpoint(DEFAULT_THERMOSTAT_SETPOINT);
  setThermostatHysteresis(DEFAULT_THERMOSTAT_HYSTERESIS);
  setThermostatLowerLimit(DEFAULT_THERMOSTAT_LOWER_LIMIT);
  setThermostatUpperLimit(DEFAULT_THERMOSTAT_UPPER_LIMIT);
}

void Config::convertFromJson(const JsonDocument& doc) {
  setOtaPass(doc["ota_pass"] | otaPass_);
  setMqttHost(doc["mqtt_host"] | mqttHost_);
  setMqttPort(doc["mqtt_port"] | mqttPort_);
  setMqttUser(doc["mqtt_user"] | mqttUser_);
  setMqttPass(doc["mqtt_pass"] | mqttPass_);
  setMqttClientId(doc["mqtt_client_id"] | mqttClientId_);
  setMqttPubTopic(doc["mqtt_pub_topic"] | mqttPubTopic_);
  setMqttSubTopic(doc["mqtt_sub_topic"] | mqttSubTopic_);
  setCron(LAMP_ON_CRON_IDX,
          doc["crons"][LAMP_ON_CRON_IDX] | crons_[LAMP_ON_CRON_IDX]);
  setCron(LAMP_OFF_CRON_IDX,
          doc["crons"][LAMP_OFF_CRON_IDX] | crons_[LAMP_OFF_CRON_IDX]);
  setCron(CO2_ON_CRON_IDX,
          doc["crons"][CO2_ON_CRON_IDX] | crons_[CO2_ON_CRON_IDX]);
  setCron(CO2_OFF_CRON_IDX,
          doc["crons"][CO2_OFF_CRON_IDX] | crons_[CO2_OFF_CRON_IDX]);
  setThermostatSetpoint(doc["thermostat_setpoint"] | thermostatSetpoint_);
  setThermostatHysteresis(doc["thermostat_hysteresis"] | thermostatHysteresis_);
  setThermostatLowerLimit(doc["thermostat_lower_limit"] |
                          thermostatLowerLimit_);
  setThermostatUpperLimit(doc["thermostat_upper_limit"] |
                          thermostatUpperLimit_);
}

void Config::convertToJson(JsonDocument& doc) const {
  doc["ota_pass"] = otaPass_;
  doc["mqtt_host"] = mqttHost_;
  doc["mqtt_port"] = mqttPort_;
  doc["mqtt_user"] = mqttUser_;
  doc["mqtt_pass"] = mqttPass_;
  doc["mqtt_client_id"] = mqttClientId_;
  doc["mqtt_pub_topic"] = mqttPubTopic_;
  doc["mqtt_sub_topic"] = mqttSubTopic_;
  doc["crons"].add(crons_[LAMP_ON_CRON_IDX]);
  doc["crons"].add(crons_[LAMP_OFF_CRON_IDX]);
  doc["crons"].add(crons_[CO2_ON_CRON_IDX]);
  doc["crons"].add(crons_[CO2_OFF_CRON_IDX]);
  doc["thermostat_setpoint"] = thermostatSetpoint_;
  doc["thermostat_hysteresis"] = thermostatHysteresis_;
  doc["thermostat_lower_limit"] = thermostatLowerLimit_;
  doc["thermostat_upper_limit"] = thermostatUpperLimit_;
}
