// The Cluster object Reads the AppConfig in from SPIFFs,
// sets up the network connection and the MQTT connection.
// It knows nothing about the sensors; these are handled
// by a subclass: SensorCluster.

#pragma  once
#include "Constants.h"
#include <ESP8266WiFi.h>        // https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>        // https://github.com/tzapu/WiFiManager
#include "Log.h"                // Local
#include "MQTT.h"               // Local
#include "AppConfig.h"          // Local


class Cluster {
  public:
    Cluster();
    virtual bool init();

    String        progVersion;
    Log           logger;
    AppConfig*    config;
    WiFiClient    client;
    PubSubClient* mqttClient;
    MQTT*         mqtt;
    bool          initialized;
    
  protected:
    void configModeCallback (WiFiManager *myWiFiManager);
    void initializeWiFi(bool reset = false);
    void initializeMQTT(const char* brokerHost);
    bool initializeAppConfig();
};
