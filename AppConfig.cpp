#include "AppConfig.h"


//  Helper function for loading/logging properties
//
bool AppConfig::fetch(const char* name, const char** value) {
  if(this->get(name, value)) {
    logger.debug("AppConfig::init: %s = %s.\n", name, *value);
    return true;
  }
  else {
    logger.error("Failure getting Name property.\n");
    return false;
  }
}


//  Simple helper function to convert a boolean string to bool.
//  VERY simple: 'true' = true, all else = false.
//
bool AppConfig::toBool(const char* value) {
  return (0 == strcmp("true", value));
}

bool AppConfig::init (Log logger) {
  bool ok = this->Config::init(logger);
  if(ok) {
    const char* value;

      if(fetch("Name", &value))        { name = value; }
      if(fetch("Broker", &value))      { broker = value; }
      if(fetch("Delay", &value))       { delay = atoi(value); }
      if(fetch("VccSensor", &value))   { enableVccSensor = toBool(value); }
      if(fetch("PirSensor", &value))   { enablePirSensor = toBool(value); }
      if(fetch("LuxSensor", &value))   { enableLuxSensor = toBool(value); }
      if(fetch("TempSensor", &value))  { enableTempSensor = toBool(value); }
      if(fetch("HumSensor", &value))   { enableHumSensor = toBool(value); }
      if(fetch("BaroSensor", &value))  { enableBaroSensor = toBool(value); }
      if(fetch("VibeSensor", &value))  { enableVibeSensor = toBool(value); }
      if(fetch("RadarSensor", &value)) { enableRadarSensor = toBool(value); }
  }
  return ok;
}
