#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>

constexpr int LAMP_ON_CRON_IDX = 0;
constexpr int LAMP_OFF_CRON_IDX = 1;
constexpr int CO2_ON_CRON_IDX = 2;
constexpr int CO2_OFF_CRON_IDX = 3;

class Config {
 public:
  void mount();
  void load();
  void save();

  const char* otaPass() const;
  const char* mqttHost() const;
  uint16_t mqttPort() const;
  const char* mqttUser() const;
  const char* mqttPass() const;
  const char* mqttClientId() const;
  const char* mqttPubTopic() const;
  const char* mqttSubTopic() const;
  const char* cron(int index) const;
  float thermostatSetpoint() const;
  float thermostatHysteresis() const;
  float thermostatLowerLimit() const;
  float thermostatUpperLimit() const;

  void setOtaPass(const char* value);
  void setMqttHost(const char* value);
  void setMqttPort(uint16_t value);
  void setMqttUser(const char* value);
  void setMqttPass(const char* value);
  void setMqttClientId(const char* value);
  void setMqttPubTopic(const char* value);
  void setMqttSubTopic(const char* value);
  void setCron(int index, const char* value);
  void setThermostatSetpoint(float value);
  void setThermostatHysteresis(float value);
  void setThermostatLowerLimit(float value);
  void setThermostatUpperLimit(float value);

 private:
  static constexpr int cronLength = 4;

  char otaPass_[16];
  char mqttHost_[64];
  uint16_t mqttPort_;
  char mqttUser_[32];
  char mqttPass_[32];
  char mqttClientId_[32];
  char mqttPubTopic_[32];
  char mqttSubTopic_[32];
  char crons_[cronLength][32];
  float thermostatSetpoint_;
  float thermostatHysteresis_;
  float thermostatLowerLimit_;
  float thermostatUpperLimit_;

  void applyDefaults();
  bool readFile(JsonDocument& doc);
  bool writeFile(const JsonDocument& doc);
  void convertFromJson(const JsonDocument& doc);
  void convertToJson(JsonDocument& doc) const;
};

extern Config AppConfig;

#endif  // CONFIG_H
