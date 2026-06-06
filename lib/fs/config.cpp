#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <config.h>

constexpr const char* CONFIG_PATH = "/config.json";
constexpr const char* DEFAULT_OTA_PASS = "admin";
constexpr const char* DEFAULT_MQTT_HOST = "mqtt3.thingspeak.com";
constexpr uint16_t DEFAULT_MQTT_PORT = 1883;
constexpr const char* DEFAULT_MQTT_USER = "";
constexpr const char* DEFAULT_MQTT_PASS = "";
constexpr const char* DEFAULT_MQTT_CLIENT_ID = "";

Config AppConfig;

void Config::applyDefaults() {
  strncpy(otaPass_, DEFAULT_OTA_PASS, kOtaPassMaxLen);
  otaPass_[kOtaPassMaxLen] = '\0';

  strncpy(mqttHost_, DEFAULT_MQTT_HOST, kMqttHostMaxLen);
  mqttHost_[kMqttHostMaxLen] = '\0';

  mqttPort_ = DEFAULT_MQTT_PORT;

  strncpy(mqttUser_, DEFAULT_MQTT_USER, kMqttUserMaxLen);
  mqttUser_[kMqttUserMaxLen] = '\0';

  strncpy(mqttPass_, DEFAULT_MQTT_PASS, kMqttPassMaxLen);
  mqttPass_[kMqttPassMaxLen] = '\0';

  strncpy(mqttClientId_, DEFAULT_MQTT_CLIENT_ID, kMqttClientIdMaxLen);
  mqttClientId_[kMqttClientIdMaxLen] = '\0';
}

bool Config::load() {
  applyDefaults();

  File file = LittleFS.open(CONFIG_PATH, "r");
  if (!file) {
    log_e("[Config] Failed to open %s", CONFIG_PATH);
    return false;
  }

  StaticJsonDocument<512> doc;
  DeserializationError err = deserializeJson(doc, file);
  size_t bytesRead = file.size();
  file.close();

  if (err) {
    log_e("[Config] Parse error: %s", err.c_str());
    return false;
  }

  log_i("[Config] Loaded %u bytes from %s", bytesRead, CONFIG_PATH);

  strncpy(otaPass_, doc["ota_pass"] | DEFAULT_OTA_PASS, kOtaPassMaxLen);
  otaPass_[kOtaPassMaxLen] = '\0';

  strncpy(mqttHost_, doc["mqtt_host"] | DEFAULT_MQTT_HOST, kMqttHostMaxLen);
  mqttHost_[kMqttHostMaxLen] = '\0';

  mqttPort_ = doc["mqtt_port"] | DEFAULT_MQTT_PORT;

  strncpy(mqttUser_, doc["mqtt_user"] | DEFAULT_MQTT_USER, kMqttUserMaxLen);
  mqttUser_[kMqttUserMaxLen] = '\0';

  strncpy(mqttPass_, doc["mqtt_pass"] | DEFAULT_MQTT_PASS, kMqttPassMaxLen);
  mqttPass_[kMqttPassMaxLen] = '\0';

  strncpy(mqttClientId_, doc["mqtt_client_id"] | DEFAULT_MQTT_CLIENT_ID,
          kMqttClientIdMaxLen);
  mqttClientId_[kMqttClientIdMaxLen] = '\0';

  return true;
}

bool Config::save() {
  StaticJsonDocument<512> doc;
  doc["ota_pass"] = otaPass_;
  doc["mqtt_host"] = mqttHost_;
  doc["mqtt_port"] = mqttPort_;
  doc["mqtt_user"] = mqttUser_;
  doc["mqtt_pass"] = mqttPass_;
  doc["mqtt_client_id"] = mqttClientId_;

  File file = LittleFS.open(CONFIG_PATH, "w");
  if (!file) {
    log_e("[Config] Failed to open %s for writing", CONFIG_PATH);
    return false;
  }

  size_t bytesWritten = serializeJson(doc, file);
  file.close();

  if (bytesWritten == 0) {
    log_e("[Config] Failed to write to %s", CONFIG_PATH);
    return false;
  }

  log_i("[Config] Saved %u bytes to %s", bytesWritten, CONFIG_PATH);
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

void Config::setOtaPass(const char* value) {
  strncpy(otaPass_, value, kOtaPassMaxLen);
  otaPass_[kOtaPassMaxLen] = '\0';
}

void Config::setMqttHost(const char* value) {
  strncpy(mqttHost_, value, kMqttHostMaxLen);
  mqttHost_[kMqttHostMaxLen] = '\0';
}

void Config::setMqttPort(uint16_t value) {
  mqttPort_ = value;
}

void Config::setMqttUser(const char* value) {
  strncpy(mqttUser_, value, kMqttUserMaxLen);
  mqttUser_[kMqttUserMaxLen] = '\0';
}

void Config::setMqttPass(const char* value) {
  strncpy(mqttPass_, value, kMqttPassMaxLen);
  mqttPass_[kMqttPassMaxLen] = '\0';
}

void Config::setMqttClientId(const char* value) {
  strncpy(mqttClientId_, value, kMqttClientIdMaxLen);
  mqttClientId_[kMqttClientIdMaxLen] = '\0';
}
