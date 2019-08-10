// New SSC5 design, intended for Wemos (LOLIN) D1 Mini
// Use WiFiManager to connect to network.
// Use Params.dat in SPIFFS for configuration

#include      <limits.h>             // Supplied LONG_MAX
#include      <ESP8266WiFi.h>        // https://github.com/esp8266/Arduino
#include      <DNSServer.h>
#include      <ESP8266WebServer.h>
#include      <WiFiManager.h>        // https://github.com/tzapu/WiFiManager
#include      "Log.h"                // Local
#include      "MQTT.h"               // Local
#include      "AppConfig.h"          // Local
#include      "VccSensor.h"          // Local
#include      "PirSensor.h"          // Local
#include      "LuxSensor.h"          // Local
#include      "TempSensor.h"         // Local
#include      "HumSensor.h"          // Local


#define       BAUD_RATE              115200
#define       LOG_LEVEL              LOG_INFO
#define       AP_NAME_PREFIX         "SSC5_"


String        progVersion           = "0.6.0";
Log           Log;                  // From Log.h (You cannot name it "log"; conflicts with math fucntion double log(double))
AppConfig     config;               // From Config.h, using default SPIFFS file name.
WiFiClient    client;
PubSubClient  mqttClient(client);
MQTT          mqtt(mqttClient);


VccSensor*    vccSensor;
PirSensor*    pirSensor;
LuxSensor*    luxSensor;
TempSensor*   tmpSensor;
HumSensor*    humSensor;

bool          initialized           = false;            // Set to true if Config works as expected


// Callback from WiFiManager when the AP is being started for picking network.
//
void configModeCallback (WiFiManager *myWiFiManager) {
  // if you used auto generated SSID, print it
  Log.info("Connect to %s\n", myWiFiManager->getConfigPortalSSID().c_str());
}


// Set up WiFi using WiFiManager. Don't return until a domain has been joined.
//
void initializeWiFi(bool reset = false) {
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset saved settings (for testing)
  if(reset) {
    wifiManager.resetSettings();
  }

  //set custom ip for portal
  wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,0,0,0));
  // Allow us to set display when in AP mode
  // wifiManager.setAPCallback(configModeCallback);

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  wifiManager.setMinimumSignalQuality();

  // Fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the name
  // ESP + ChipID and goes into a blocking loop awaiting configuration
  Log.info("Connecting...");
  String apName = AP_NAME_PREFIX;
  apName += config.name();
  wifiManager.autoConnect(apName.c_str());
  
  //if you get here you have connected to the WiFi
  Log.info("\n");
  Log.info("Connected to ");
  Log.info(WiFi.SSID().c_str());  // Tell us what network we're connected to
  Log.info("\n");
  Log.info("IP address:\t");
  IPAddress ip = WiFi.localIP();
  Log.info(ip.toString().c_str());           // Send the IP address of the ESP8266 to the computer
  Log.info("\n\n");
}


// Find and initalize MQTT
//
void initializeMQTT(const char* brokerHost) {
  // One time only: look up MQTT Broker IP Address from name:
  IPAddress addr;
  bool hasMQTT = WiFi.hostByName(brokerHost, addr);
  if(hasMQTT) {
    Log.info("Initializing MQTT.\n");
    mqtt.config(config.name(), addr.toString(), 1883, false);
  }
}


// Initialize the configuration syste,m reading settings from SPIFFS.
// Return false if initialization failed.
//
bool initializeConfig() {
  bool initialized = config.init();
  if(initialized) {
    Log.info("Configuration system initialized.\n");
    return initialized;
  }
  else {
    Log.error("Failed to initialize Configuration.\n");
    return false;
  }
}


void initializeSensors() {
    pirSensor = new PirSensor(config, mqtt, Log, "pir");
    if(!pirSensor->init()) {
      Log.error("Error initializing PIR Sensor\n");
    }
    vccSensor = new VccSensor(config, mqtt, Log, "vcc");
    if(!vccSensor->init()) {
      Log.error("Error initializing VCC Sensor\n");
    }
    luxSensor = new LuxSensor(config, mqtt, Log, "lux");
    if(!luxSensor->init()) {
      Log.error("Error initializing Lux Sensor\n");
    }
    tmpSensor = new TempSensor(config, mqtt, Log, "temp");
    if(!tmpSensor->init()) {
      Log.error("Error initializing Tempurature Sensor\n");
    }
    humSensor = new HumSensor(config, mqtt, Log, "hum");
    if(!humSensor->init()) {
      Log.error("Error initializing Humidity Sensor\n");
    }
}


void setup() {
  Log.init(BAUD_RATE, LOG_LEVEL);
  Log.info("\nSystem startup: version %s\n", progVersion.c_str());
  initialized = initializeConfig();
  if(initialized) {
    Log.info("Configuration successful.\n");
    initializeSensors();
    initializeWiFi();
    initializeMQTT(config.broker());
  }
}

void loop() {
  if(!initialized) {
    Log.error("System not initialized/configured.\n");
    delay(60000);
    initialized = initializeConfig();
    return;
  }

  // Calculate how long to keep looping:
  time_t loop_limit = millis() + config.delay();

  vccSensor->begin();
  pirSensor->begin();
  luxSensor->begin();
  tmpSensor->begin();
  humSensor->begin();
  
  while(millis() < loop_limit) {
    mqtt.ensureConnection();

    vccSensor->read();
    pirSensor->read();
    luxSensor->read();
    tmpSensor->read();
    humSensor->read();
    delay(1000);
  }

  vccSensor->report();
  pirSensor->report();
  luxSensor->report();
  tmpSensor->report();
  humSensor->report();
}
