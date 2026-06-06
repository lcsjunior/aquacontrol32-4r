#include <mqtt_client.h>

#define MQTT_CONN_TIMEOUT_MS 5000UL

MQTTClient MQTT;

void MQTTClient::begin(Client& espClient, const char* server, uint16_t port,
                       const char* clientId, const char* username,
                       const char* password) {
  pubSubClient_.setClient(espClient);
  pubSubClient_.setServer(server, port);
  server_ = server;
  clientId_ = clientId;
  username_ = username;
  password_ = password;
  subscribedTopic_ = nullptr;
  lastConnectAttempt_ = millis() - MQTT_CONN_TIMEOUT_MS;
}

void MQTTClient::begin(Client& espClient, const Config& config) {
  begin(espClient, config.mqttHost(), config.mqttPort(), config.mqttClientId(),
        config.mqttUser(), config.mqttPass());
}

bool MQTTClient::connect() {
  log_i("[MQTTClient] Attempting connection to %s", server_);
  bool ok = pubSubClient_.connect(clientId_, username_, password_);
  if (!ok) {
    log_w("[MQTTClient] connect failed, rc=%d, retry in %lu ms",
          pubSubClient_.state(), MQTT_CONN_TIMEOUT_MS);
    return false;
  }
  log_i("[MQTTClient] Connected to broker %s", server_);
  if (subscribedTopic_ == nullptr)
    return true;
  pubSubClient_.subscribe(subscribedTopic_);
  log_i("[MQTTClient] Re-subscribed to %s", subscribedTopic_);
  return true;
}

void MQTTClient::subscribe(const char* topic) {
  if (!topic || topic[0] == '\0') {
    log_w("[MQTTClient] subscribe called with empty topic, skipping");
    return;
  }
  subscribedTopic_ = topic;
  pubSubClient_.subscribe(topic);
  log_i("[MQTTClient] Subscribed to %s", topic);
}

void MQTTClient::publish(const char* topic, const char* payload) {
  if (!topic || topic[0] == '\0') {
    log_w("[MQTTClient] publish called with empty topic, skipping");
    return;
  }
  const size_t len = strlen(payload);
  log_i("[MQTTClient] Publishing to %s (%u bytes)", topic, (unsigned)len);
  pubSubClient_.publish(topic, payload);
}

bool MQTTClient::reconnect() {
  if (millis() - lastConnectAttempt_ < MQTT_CONN_TIMEOUT_MS)
    return false;
  lastConnectAttempt_ = millis();
  return connect();
}

void MQTTClient::loop() {
  if (!pubSubClient_.connected())
    reconnect();
  pubSubClient_.loop();
}
