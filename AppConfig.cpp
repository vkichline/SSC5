#include "AppConfig.h"
#include "Log.h"

extern Log Log;


bool AppConfig::init () {
  bool ok = this->Config::init();
  if(ok) {
    const char* value;

    // Name property
    if(this->get("Name", &value)) {
      Log.debug("AppConfig::init: %s = %s.\n", "Name", value);
      _name = value;
    }
    else {
      Log.error("Failure getting Name property.\n");
    }

    // Broker property
    if(this->get("Broker", &value)) {
      Log.debug("AppConfig::init: %s = %s.\n", "Broker", value);
      _broker = value;
    }
    else {
      Log.error("Failure getting Broker property.\n");
    }

    // Delay property
    if(this->get("Delay", &value)) {
      Log.debug("AppConfig::init: %s = %s.\n", "Delay", value);
      _delay = atoi(value);
    }
    else {
      Log.error("Failure getting Delay property.\n");
    }
  }
  return ok;
}
