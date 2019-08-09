// New SSC5 design, intended for Wemos (LOLIN) D1 Mini
// Use WiFiManager to connect to network.
// Use Params.dat in SPIFFS for configuration

#include      <limits.h>             // Supplied LONG_MAX
#include      <ESP8266WiFi.h>        // https://github.com/esp8266/Arduino
#include      <DNSServer.h>
#include      <ESP8266WebServer.h>
#include      <WiFiManager.h>        // https://github.com/tzapu/WiFiManager
#include      <WEMOS_SHT3X.h>        // https://github.com/wemos/WEMOS_SHT3x_Arduino_Library
#include      <BH1750.h>
#include      "Log.h"                // Local
#include      "MQTT.h"               // Local
#include      "AppConfig.h"          // Local
#include      "Sensor.h"             // Local


#define       BAUD_RATE              115200
#define       LOG_LEVEL              LOG_INFO
#define       AP_NAME_PREFIX         "SSC5_"


Log           Log;                  // From Log.h (You cannot name it "log"; conflicts with math fucntion double log(double))
AppConfig     config;               // From Config.h, using default SPIFFS file name.
WiFiClient    client;
PubSubClient  mqttClient(client);
MQTT          mqtt(mqttClient);

SHT3X         sht30(0x45);                              // Tempurature/humidity shield
BH1750        light(0x23);                              // Ambient light shield
const int     PIR                   = D3;               // Wemos PIR Shield

bool          initialized           = false;            // Set to true if Config works as expected
String        progVersion           = "0.4.1";
String        feed_Vcc;                                 // Set up in initMQTTFeeds
String        feed_Temp;                                // Set up in initMQTTFeeds
String        feed_Hum;                                 // Set up in initMQTTFeeds
String        feed_Lux;                                 // Set up in initMQTTFeeds
String        feed_Pir;                                 // Set up in initMQTTFeeds


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


// Once the configuration has been read, set up the global feed names.
//
void initMQTTFeeds() {
  feed_Vcc = "sensors/";
  feed_Vcc += config.name();
  feed_Vcc += "/vcc";
  Log.info("feed_Vcc set to %s\n", feed_Vcc.c_str());
  feed_Temp = "sensors/";
  feed_Temp += config.name();
  feed_Temp += "/temp";
  Log.info("feed_Temp set to %s\n", feed_Temp.c_str());
  feed_Hum = "sensors/";
  feed_Hum += config.name();
  feed_Hum += "/hum";
  Log.info("feed_Hum set to %s\n", feed_Hum.c_str());
  feed_Lux = "sensors/";
  feed_Lux += config.name();
  feed_Lux += "/lux";
  Log.info("feed_Lux set to %s\n", feed_Lux.c_str());
  feed_Pir = "sensors/";
  feed_Pir += config.name();
  feed_Pir += "/pir";
  Log.info("feed_Pir set to %s\n", feed_Pir.c_str());
}


// Find and initalize MQTT
//
void initializeMQTT(const char* brokerHost) {
  // One time only: look up MQTT Broker IP Address from name:
  IPAddress addr;
  bool hasMQTT = WiFi.hostByName(brokerHost, addr);
  if(hasMQTT) {
    initMQTTFeeds();
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


ADC_MODE(ADC_VCC);  // Set up chip to report VCC voltage on AO using ESP.getVcc()

void setup() {
  pinMode(PIR, INPUT);  // Wemos PIR shield, digial input
  Log.init(BAUD_RATE, LOG_LEVEL);
  Log.info("\nSystem startup: version %s\n", progVersion.c_str());
  initialized = initializeConfig();
  if(initialized) {
    Log.info("Configuration successful.\n");
    initializeWiFi();
    initializeMQTT(config.broker());
  }

  // Test code:
  Sensor* sensor = new Sensor(config, mqtt, Log, "test");
}

void loop() {
  if(!initialized) {
    Log.error("System not initialized/configured.\n");
    delay(60000);
    return;
  }

  // Calculate how long to keep looping:
  time_t loop_limit = millis() + config.delay();
  int    loop_count = 0;
  float  vcc        = 0.0;
  float  temp       = 0.0;
  float  hum        = 0.0;
  long   lux        = 0;
  int    pir        = 0;
  
  while(millis() < loop_limit) {
    mqtt.ensureConnection();

    // Read the PIR didital state and accumulate average
    if(HIGH == digitalRead(PIR)) {
      pir++;
    }
    // Detect and accumulate the VCC voltage level:
    vcc += (ESP.getVcc() / 1000.0);
    if(0 == sht30.get()) {
      // Detect and accumulate the tempurature
      temp += sht30.fTemp;
      // Detect and accumulate humidity
      hum += sht30.humidity;
    }
    else {
      Log.error("Error reading SHT30 sensor.\n");
    }
    if(light.begin()) {
      lux += light.readLightLevel();
    }
    else {
      Log.error("Error reading BH1750 sensor.\n");
    }

    loop_count++;
    Log.debug("Sample loop. Count = %d, vcc = %f, temp = %f, hum = %f, lux = %d, pir = %d\n", loop_count, vcc, temp, hum, lux, pir);
    delay(1000);
  }

  if(0 < loop_count) {
    // Report Vcc
    vcc /= (float)loop_count;
    Log.info("VCC: %3.2fV\n", vcc);
    mqtt.publishFloat(feed_Vcc, vcc, 2);
  
    // Report the tempurature
    temp /= (float)loop_count;
    Log.info("Temp: %3.2f F\n", temp);
    mqtt.publishFloat(feed_Temp, temp, 1);
  
    // Report the humidity
    hum /= (float)loop_count;
    Log.info("Hum: %3.2f%% RH\n", hum);
    mqtt.publishFloat(feed_Hum, hum, 1);

    // Report the light level
    lux /= loop_count;
    Log.info("Lux: %d Lux\n", lux);
    mqtt.publishInt(feed_Lux, lux);

    // Report the average PIR on time as %
    float fPir = (float)pir * 100.0 / (float)loop_count;
    Log.info("PIR: %3.2f%%\n", fPir);
    mqtt.publishFloat(feed_Pir, fPir, 1);
  }
}