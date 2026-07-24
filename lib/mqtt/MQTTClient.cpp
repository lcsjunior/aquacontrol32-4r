#include "MQTTClient.h"

#define MQTT_CONN_TIMEOUT_MS 10000UL

MQTTClient MQTT;

void MQTTClient::begin(const char* host, uint16_t port, const char* clientId,
                       const char* username, const char* password,
                       const char* subTopic, const char* pubTopic) {
  pubSubClient_.setClient(wifiClient_);
  pubSubClient_.setServer(host, port);
  server_ = host;
  clientId_ = clientId;
  username_ = username;
  password_ = password;
  subscribedTopic_ = subTopic;
  publishTopic_ = pubTopic;
  lastConnectAttempt_ = millis() - MQTT_CONN_TIMEOUT_MS;
}

void MQTTClient::connect() {
  log_i("Attempting connection to %s", server_);
  bool ok = pubSubClient_.connect(clientId_, username_, password_);
  if (!ok) {
    log_e("Connect failed, rc=%d, retry in %lu ms", pubSubClient_.state(),
          MQTT_CONN_TIMEOUT_MS);
    return;
  }

  log_i("Connected to broker %s", server_);
  subscribe();
}

void MQTTClient::subscribe() {
  if (!subscribedTopic_ || subscribedTopic_[0] == '\0') {
    log_w("Subscribe called with empty topic, skipping");
    return;
  }

  pubSubClient_.subscribe(subscribedTopic_);
  log_i("Subscribed to %s", subscribedTopic_);
}

void MQTTClient::publish(const char* payload) {
  if (!publishTopic_ || publishTopic_[0] == '\0') {
    log_w("Publish called with empty topic, skipping");
    return;
  }

  const size_t len = strlen(payload);
  log_i("Publishing to %s (%u bytes)", publishTopic_, (unsigned)len);
  pubSubClient_.publish(publishTopic_, payload);
}

void MQTTClient::reconnect() {
  if (millis() - lastConnectAttempt_ < MQTT_CONN_TIMEOUT_MS)
    return;
  lastConnectAttempt_ = millis();
  connect();
}

void MQTTClient::loop() {
  if (!pubSubClient_.connected())
    reconnect();
  pubSubClient_.loop();
}
