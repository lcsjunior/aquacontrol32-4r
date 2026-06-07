#include <mqtt_client.h>

#define MQTT_CONN_TIMEOUT_MS 10000UL

MQTTClient MQTT;

void MQTTClient::begin(Client& espClient, const Config& config) {
  pubSubClient_.setClient(espClient);
  pubSubClient_.setServer(config.mqttHost(), config.mqttPort());
  server_ = config.mqttHost();
  clientId_ = config.mqttClientId();
  username_ = config.mqttUser();
  password_ = config.mqttPass();
  subscribedTopic_ = config.mqttPubTopic();
  lastConnectAttempt_ = millis() - MQTT_CONN_TIMEOUT_MS;
}

bool MQTTClient::connect() {
  log_i("Attempting connection to %s", server_);
  bool ok = pubSubClient_.connect(clientId_, username_, password_);
  if (!ok) {
    log_w("Connect failed, rc=%d, retry in %lu ms", pubSubClient_.state(),
          MQTT_CONN_TIMEOUT_MS);
    return false;
  }
  log_i("Connected to broker %s", server_);
  subscribe(subscribedTopic_);
  return true;
}

void MQTTClient::subscribe(const char* topic) {
  if (topic[0] == '\0') {
    log_w("Subscribe called with empty topic, skipping");
    return;
  }
  subscribedTopic_ = topic;
  pubSubClient_.subscribe(topic);
  log_i("Subscribed to %s", topic);
}

void MQTTClient::publish(const char* topic, const char* payload) {
  if (topic[0] == '\0') {
    log_w("Publish called with empty topic, skipping");
    return;
  }
  const size_t len = strlen(payload);
  log_i("Publishing to %s (%u bytes)", topic, (unsigned)len);
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
