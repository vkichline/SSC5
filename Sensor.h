//  Base class for all sensors

#include  "Log.h"       // For reporting status and issues to output
#include  "MQTT.h"      // For reporting values to message broker
#include  "AppConfig.h" // Needed to construct feed name, probably other things as well.

#ifndef FILE_SENSOR_H
#define FILE_SENSOR_H


class Sensor {
  public:
    // Initialize the sensor with mqtt provider, log object and sensor name.
    Sensor(AppConfig& configParam, MQTT& mqttParam, Log& logParam, const char* nameParam) :
      config(configParam), mqtt(mqttParam), logger(logParam), name(nameParam)
    {
      this->feedName = "sensors/";
      this->feedName += config.name;
      this->feedName += "/";
      this->feedName += this->name;
      logger.info("Created Sensor '%s' with feed name '%s'\n", this->name, this->feedName.c_str());
    }
    virtual bool init() { return true; }    // Called once, in setup. Return success of initialization
    
    virtual void begin() {}                 // Called at beginning of aquisition loop.
    virtual void read() {}                  // Called repeadedly durring aquisition.
    virtual void report() {}                // Called at end of acquisition loop. Reports data to mqtt.

  protected:
    AppConfig&  config;
    MQTT&       mqtt;
    Log&        logger;
    const char* name;
    String      feedName;
};

#endif // FILE_SENSOR_H
