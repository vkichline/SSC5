//  Config System
// This class reads a Config.dat file from SPIFFS
// and makes a linked list of name/value properties.
// While an associative array would be perfect, intially
// the Config object will simply read name/value pairs.
// A subclass can be defined for each application, providing
// simplified access (see AppConfig.)
// Config.dat:
// Saved in SPIFFS at the root ("/Config.dat")
// Lines starting with "*" are comments.
// Other lines start with "Cccc=" where Cccc is a property name.
// Property value immediately follows the "="
// Lines are terminated with a line feed.
// All values are strings, but can be converted to different types with helpers.
// Example:
//  * This is an example config file.
//  NAME=TestRig
//  BROKER=ThingOne.local
//  DELAY=30000
//  VccSensor=true
//  PirSensor=true
//  LuxSensor=true
//  TempSensor=true
//  HumSensor=true
//  BaroSensor=false
//  VibeSensor=false
//
// Class usage:
// Class is initialized with an optional config file name:
// Config config = new Config(); or
// Config config = new Config("Special.name");
//
// Data is read from SPIFFS by calling init(), which returns boolean success.
// bool initialized = config.init();
// All values are read into a linked list of ConfigParams.
//
// Configuration data is retrieved by property name. The boolean result is true if valid.
// If null is provided for the string pointer, it is not modified, only the return value is set.
// If the accessor returns true and a const char** is provided, the char* is set to the
// c_str of the value pointer.
// bool valid = config.get("DELAY", &valPtr);

#ifndef FILE_CONFIG_H
#define FILE_CONFIG_H


#include <WString.h>

struct ConfigParam {
    ConfigParam(const char* name, const char* value) {
      this->name = name;
      this->value = value;
      this->next = NULL;
    }
  
    void link(ConfigParam* pNext) {
      this->next = pNext;
    }
    String        name;
    String        value;
    ConfigParam*  next;
};


class Config {
  public:
    Config(const char* configFileName);
    Config();
  
    bool init();
    bool get(const char* name, const char** value);

  protected:
    void add(ConfigParam* param);
    
  private:
    String        configFileName;
    ConfigParam*  head = NULL;
    ConfigParam*  tail = NULL;
};

#endif // FILE_CONFIG_H
