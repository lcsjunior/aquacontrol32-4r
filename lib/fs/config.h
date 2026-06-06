#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

class Config {
 public:
  bool load();
  bool save();

  const char* otaPass() const;
  const char* mqttHost() const;
  uint16_t mqttPort() const;
  const char* mqttUser() const;
  const char* mqttPass() const;
  const char* mqttClientId() const;

  void setOtaPass(const char* value);
  void setMqttHost(const char* value);
  void setMqttPort(uint16_t value);
  void setMqttUser(const char* value);
  void setMqttPass(const char* value);
  void setMqttClientId(const char* value);

 private:
  static constexpr int kOtaPassMaxLen = 32;
  static constexpr int kMqttHostMaxLen = 64;
  static constexpr int kMqttUserMaxLen = 48;
  static constexpr int kMqttPassMaxLen = 48;
  static constexpr int kMqttClientIdMaxLen = 48;

  char otaPass_[kOtaPassMaxLen + 1];
  char mqttHost_[kMqttHostMaxLen + 1];
  uint16_t mqttPort_;
  char mqttUser_[kMqttUserMaxLen + 1];
  char mqttPass_[kMqttPassMaxLen + 1];
  char mqttClientId_[kMqttClientIdMaxLen + 1];

  void applyDefaults();
};

extern Config AppConfig;

#endif  // CONFIG_H
