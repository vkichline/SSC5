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
//
// V2: No String objects are created in ConfigParams. The data buffer is read in once, and preserved.
// The buffer is modified in memory to replace = signs and <cr>s with NULLs for in-place strings.
// All references to names and values point to the buffer.  DO NOT MODIFY THEM.
// This makes comments more expensive, but is the most memory efficient and causes least heap fragmentation.
// The system is not expected to be initialized more than once. A guard is built in.

#pragma once
#include <WString.h>  // Used for NULL, strcmp, etc.


struct ConfigParam {
    ConfigParam(const char* pName, const char* pValue) {
      name  = pName;
      value = pValue;
      next  = NULL;
    }
  
    void link(ConfigParam* pNext) {
      next = pNext;
    }
    const char*   name;
    const char*   value;
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
    char*         configFileName;
    bool          configured  = false;
    ConfigParam*  head        = NULL;
    ConfigParam*  tail        = NULL;
};
