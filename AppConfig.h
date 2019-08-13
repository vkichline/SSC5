// Subclass of Config, customized for this SCC5 implementation.

#ifndef FILE_APPCONFIG_H
#define FILE_APPCONFIG_H

#include "Config.h"

class AppConfig : Config {
  public:
    AppConfig() : Config() {}
    AppConfig(const char* configFileName) : Config(configFileName) {}
  
    // In our init function, load the config file, then fish out all the
    // properties we need and set up accessors to them.
    bool init ();

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

#endif // FILE_APPCONFIG_H
