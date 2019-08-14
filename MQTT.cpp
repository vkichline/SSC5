#include "MQTT.h"


MQTT::MQTT(PubSubClient* client, Log logger) {
  this->mqttClient = client;
  this->logger     = logger;
}


void MQTT::config(String clientID, String broker, int port, bool suppress) {
  this->clientID = clientID;
  this->broker   = broker;
  this->port     = port;
  this->suppress = suppress;
  if(suppress) {
    logger.info("Not using MQTT: suppressed mode.\n");
  }
}


void MQTT::ensureConnection() {
  if(suppress) return;
  mqttClient->loop();

  // Loop until we're reconnected
  while (!mqttClient->connected()) {
    mqttClient->setServer(broker.c_str(), port);
     logger.info("MQTT server set to: %s:%d\n", broker.c_str(), port); 
  
    // Attempt to connect
    if (mqttClient->connect(clientID.c_str())) {
      logger.info("MQTT connected, Client name = %s\n\n", clientID.c_str());
    } else {
      logger.error("ERROR: Connecton failed, rc = %d\n", mqttClient->state());
      logger.info("try again in 5 seconds.\n");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


bool MQTT::idle() {
  bool result = true;
  if(!suppress) {
    // Ping the message broker; stay alive
    result = mqttClient->loop();
  }
  return result;
}


void MQTT::doPublishing(String topic, String val) {
  logger.info("%sPublishing %s to %s\n", (suppress) ? "Not ": "", val.c_str(), topic.c_str());
  if(!suppress) {
    bool result = mqttClient->publish(topic.c_str(), val.c_str());
    if(!result) {
      logger.error("ERROR: Publish to topic %s failed.\n", topic.c_str());
    }
  }
}


void MQTT::publishString(String topic, String val) {
  val.trim();
  if (0 == val.length()) {
    logger.error("ERROR: Skipping bad value on %s\n", topic.c_str());
  }
  else {
    doPublishing(topic, val);
  }
}


void MQTT::publishFloat(String topic, float val, int decimalPlaces) {
  String s = String(val, decimalPlaces);
  s.trim();
  if (s.equals("nan")) {
    logger.error("ERROR: Skipping nan value on %s\n", topic.c_str());
  }
  else {
    doPublishing(topic, s);
  }
}


void MQTT::publishInt(String topic, int val) {
  String s = String(val);
  s.trim();
  doPublishing(topic, s);
}
