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

    const char* name() {
      return this->_name.c_str();
    }

    const char* broker() {
      return this->_broker.c_str();
    }

    int delay() {
      return this->_delay;
    }

  private:
    String  _name;
    String  _broker;
    int     _delay;
};

#endif FILE_APPCONFIG_H
