#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>
#include <Client.h>
#include <PubSubClient.h>

class MQTTClient {
 public:
  void begin(Client& espClient, const char* server, int port,
             const char* clientId, const char* username, const char* password);

  bool connect();
  void publish(const char* topic, const char* payload);
  void subscribe(const char* topic);
  void loop();

 private:
  PubSubClient pubSubClient_;
  const char* server_ = nullptr;
  const char* clientId_ = nullptr;
  const char* username_ = nullptr;
  const char* password_ = nullptr;
  const char* subscribedTopic_ = nullptr;
  unsigned long lastConnectAttempt_ = 0;

  bool reconnect();
};

extern MQTTClient MQTT;

#endif  // MQTT_CLIENT_H
