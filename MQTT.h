#pragma once
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Log.h"


class MQTT {
  public:
    MQTT(PubSubClient client);
    void config(String clientID, String broker, int port, bool suppress);
    void ensureConnection();
    bool idle();
    void doPublishing(String topic, String val);
    void publishString(String topic, String val);
    void publishFloat(String topic, float val, int decimalPlaces = 0);
    void publishInt(String topic, int val);

  private:
    PubSubClient  mqttClient;
    String        clientID;
    String        broker;
    int           port;
    bool          suppress = false;
};

