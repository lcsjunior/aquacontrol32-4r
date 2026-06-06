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
constexpr const char* DEFAULT_MQTT_PUB_TOPIC = "";
constexpr const char* DEFAULT_MQTT_SUB_TOPIC = "";
constexpr const char* DEFAULT_CRON_EXPR = "";

Config AppConfig;

void Config::applyDefaults() {
  setOtaPass(DEFAULT_OTA_PASS);
  setMqttHost(DEFAULT_MQTT_HOST);
  setMqttPort(DEFAULT_MQTT_PORT);
  setMqttUser(DEFAULT_MQTT_USER);
  setMqttPass(DEFAULT_MQTT_PASS);
  setMqttClientId(DEFAULT_MQTT_CLIENT_ID);
  setMqttPubTopic(DEFAULT_MQTT_PUB_TOPIC);
  setMqttSubTopic(DEFAULT_MQTT_SUB_TOPIC);
  for (int i = 0; i < kCronCount; ++i) {
    setCron(i, DEFAULT_CRON_EXPR);
  }
}

bool Config::mount() {
  if (!LittleFS.begin(true)) {
    log_w("[Config] Failed to mount LittleFS");
    return false;
  }
  return true;
}

bool Config::load() {
  applyDefaults();

  File file = LittleFS.open(CONFIG_PATH, "r");
  if (!file) {
    log_e("[Config] Failed to open %s", CONFIG_PATH);
    return false;
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, file);
  size_t bytesRead = file.size();
  file.close();

  if (err) {
    log_e("[Config] Parse error: %s", err.c_str());
    return false;
  }

  log_i("[Config] Loaded %u bytes from %s", bytesRead, CONFIG_PATH);

  convertFromJson(doc);

  return true;
}

void Config::convertFromJson(const JsonDocument& doc) {
  setOtaPass(doc["ota_pass"] | DEFAULT_OTA_PASS);
  setMqttHost(doc["mqtt_host"] | DEFAULT_MQTT_HOST);
  setMqttPort(doc["mqtt_port"] | DEFAULT_MQTT_PORT);
  setMqttUser(doc["mqtt_user"] | DEFAULT_MQTT_USER);
  setMqttPass(doc["mqtt_pass"] | DEFAULT_MQTT_PASS);
  setMqttClientId(doc["mqtt_client_id"] | DEFAULT_MQTT_CLIENT_ID);
  setMqttPubTopic(doc["mqtt_pub_topic"] | DEFAULT_MQTT_PUB_TOPIC);
  setMqttSubTopic(doc["mqtt_sub_topic"] | DEFAULT_MQTT_SUB_TOPIC);
  for (size_t i = 0; i < kCronCount; ++i) {
    setCron(i, doc["crons"][i] | DEFAULT_CRON_EXPR);
  }
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
  for (int i = 0; i < kCronCount; ++i) {
    doc["crons"].add(crons_[i]);
  }
}

bool Config::save() {
  JsonDocument doc;
  convertToJson(doc);

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
const char* Config::mqttPubTopic() const {
  return mqttPubTopic_;
}
const char* Config::mqttSubTopic() const {
  return mqttSubTopic_;
}
const char* Config::cron(int index) const {
  if (index < 0 || index >= kCronCount) {
    return "";
  }
  return crons_[index];
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

void Config::setMqttPubTopic(const char* value) {
  strncpy(mqttPubTopic_, value, kMqttTopicMaxLen);
  mqttPubTopic_[kMqttTopicMaxLen] = '\0';
}

void Config::setMqttSubTopic(const char* value) {
  strncpy(mqttSubTopic_, value, kMqttTopicMaxLen);
  mqttSubTopic_[kMqttTopicMaxLen] = '\0';
}

void Config::setCron(int index, const char* value) {
  if (index < 0 || index >= kCronCount) {
    return;
  }
  strncpy(crons_[index], value, kCronExprMaxLen);
  crons_[index][kCronExprMaxLen] = '\0';
}
