#include "Cluster.h"

Cluster::Cluster() {
  initialized = false;
  progVersion = "0.8.1";
  mqttClient  = new PubSubClient(client);
  mqtt        = new MQTT(mqttClient, logger);
  config      = new AppConfig();
}


// Callback from WiFiManager when the AP is being started for picking network.
//
void Cluster::configModeCallback (WiFiManager *myWiFiManager) {
  // if you used auto generated SSID, print it
  logger.info("Connect to %s\n", myWiFiManager->getConfigPortalSSID().c_str());
}


// Set up WiFi using WiFiManager. Don't return until a domain has been joined.
//
void Cluster::initializeWiFi(bool reset) {
  //WiFiManager
  //Local initialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset saved settings (for testing)
  if(reset) {
    wifiManager.resetSettings();
  }

  //set custom ip for portal
  wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,0,0,0));
  // Allow us to set display when in AP mode
  // wifiManager.setAPCallback(configModeCallback);

  //set minimum quality of signal so it ignores AP's under that quality
  //defaults to 8%
  wifiManager.setMinimumSignalQuality();

  // Fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the name
  // ESP + ChipID and goes into a blocking loop awaiting configuration
  logger.info("Connecting...");
  String apName = AP_NAME_PREFIX;
  apName += config->name;
  wifiManager.autoConnect(apName.c_str());
  
  //if you get here you have connected to the WiFi
  logger.info("\n");
  logger.info("Connected to ");
  logger.info(WiFi.SSID().c_str());  // Tell us what network we're connected to
  logger.info("\n");
  logger.info("IP address:\t");
  IPAddress ip = WiFi.localIP();
  logger.info(ip.toString().c_str()); // Send the IP address of the ESP8266 to the computer
  logger.info("\n\n");
}


// Find and initialize MQTT
//
void Cluster::initializeMQTT(const char* brokerHost) {
  // One time only: look up MQTT Broker IP Address from name:
  IPAddress addr;
  bool hasMQTT = WiFi.hostByName(brokerHost, addr);
  if(hasMQTT) {
    logger.info("Initializing MQTT.\n");
    mqtt->config(config->name, addr.toString(), 1883, false);
  }
}


// Initialize the configuration system reading settings from SPIFFS.
// Return false if initialization failed.
//
bool Cluster::initializeAppConfig() {
  bool initialized = config->init(logger);
  if(initialized) {
    logger.info("Configuration system initialized.\n");
  }
  else {
    logger.error("Failed to initialize Configuration.\n");
  }
  return initialized;
}


bool Cluster::init() {
  logger.init(BAUD_RATE, LOG_LEVEL);
  logger.info("\nSystem startup: version %s\n", progVersion.c_str());
  initialized = initializeAppConfig();
  if(initialized) {
    logger.info("Configuration successful.\n");
    initializeWiFi();
    initializeMQTT(config->broker);
  }
  return initialized;
}
