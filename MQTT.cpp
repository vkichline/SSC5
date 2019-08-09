#include "MQTT.h"

extern Log Log;

MQTT::MQTT(PubSubClient client) {
  this->mqttClient = client;
}


void MQTT::config(String clientID, String broker, int port, bool suppress) {
  this->clientID = clientID;
  this->broker   = broker;
  this->port     = port;
  this->suppress = suppress;
  if(suppress) {
    Log.info("Not using MQTT: suppressed mode.\n");
  }
}


void MQTT::ensureConnection() {
  if(suppress) return;
  mqttClient.loop();

  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    mqttClient.setServer(broker.c_str(), port);
     Log.info("MQTT server set to: %s:%d\n", broker.c_str(), port); 
  
    // Attempt to connect
    if (mqttClient.connect(clientID.c_str())) {
      Log.info("MQTT connected, Client name = %s\n\n", clientID.c_str());
    } else {
      Log.error("ERROR: Connecton failed, rc = %d\n", mqttClient.state());
      Log.info("try again in 5 seconds.\n");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


bool MQTT::idle() {
  bool result = true;
  if(!suppress) {
    // Ping the message broker; stay alive
    result = mqttClient.loop();
  }
  return result;
}


void MQTT::doPublishing(String topic, String val) {
  Log.info("%sPublishing %s to %s\n", (suppress) ? "Not ": "", val.c_str(), topic.c_str());
  if(!suppress) {
    bool result = mqttClient.publish(topic.c_str(), val.c_str());
    if(!result) {
      Log.error("ERROR: Publish to topic %s failed.\n", topic.c_str());
    }
  }
}


void MQTT::publishString(String topic, String val) {
  val.trim();
  if (0 == val.length()) {
    Log.error("ERROR: Skipping bad value on %s\n", topic.c_str());
  }
  else {
    doPublishing(topic, val);
  }
}


void MQTT::publishFloat(String topic, float val, int decimalPlaces) {
  String s = String(val, decimalPlaces);
  s.trim();
  if (s.equals("nan")) {
    Log.error("ERROR: Skipping nan value on %s\n", topic.c_str());
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

