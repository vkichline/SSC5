// Subclass of Config, customized for this SCC5 implementation.

#pragma once
#include "Config.h"

class AppConfig : Config {
  public:
    AppConfig() : Config() {}
    AppConfig(const char* configFileName) : Config(configFileName) {}
  
    // In our init function, load the config file, then fish out all the
    // properties we need and set up accessors to them.
    bool init (Log logger);

    const char*   name;
    const char*   broker;
    int           delay;
    bool          enableVccSensor;
    bool          enablePirSensor;
    bool          enableLuxSensor;
    bool          enableTempSensor;
    bool          enableHumSensor;
    bool          enableBaroSensor;
    bool          enableVibeSensor;
    bool          enableRadarSensor;

  private:
    bool          fetch(const char* name, const char** value);
    bool          toBool(const char* value);
};
