#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

class MQTTClient {
 public:
  void begin(const char* host, uint16_t port, const char* clientId,
             const char* username, const char* password, const char* subTopic,
             const char* pubTopic);

  void connect();
  void publish(const char* payload);
  void subscribe();
  void loop();

 private:
  WiFiClient wifiClient_;
  PubSubClient pubSubClient_;
  const char* server_ = nullptr;
  const char* clientId_ = nullptr;
  const char* username_ = nullptr;
  const char* password_ = nullptr;
  const char* subscribedTopic_ = nullptr;
  const char* publishTopic_ = nullptr;
  unsigned long lastConnectAttempt_ = 0;

  void reconnect();
};

extern MQTTClient MQTT;

#endif  // MQTT_CLIENT_H
