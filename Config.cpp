#include <FS.h>
#include "Config.h"
#include "Log.h"

extern Log Log;


Config::Config(const char* configFileName) {
  this->configFileName = configFileName;
}

Config::Config() {
  this->configFileName = F("/Config.dat");
}


void Config::add(ConfigParam* param) {
  // If the list is empty, this becomes the head and the tail:
  if(NULL == this->tail) {
    this->head = this->tail = param;
  }
  // Otherwise, link param to the existing tail, and it becomes the tail.
  else {
    this->tail->link(param);
    this->tail = param;
  }
}

//  Return true if Config.dat was read successfully
//  false if not.
bool Config::init() {
  if (SPIFFS.begin()) {
    Log.debug("Config: mounted file system.\n");
    if (SPIFFS.exists(this->configFileName)) {
      Log.debug("Reading %s\n", this->configFileName.c_str());
      File configFile = SPIFFS.open(this->configFileName, "r");
      if (configFile) {
        Log.debug("Config file opened.\n");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        char* buf = new char[size+1];
        buf[size] = 0;

        configFile.readBytes(buf, size);
        configFile.close();
        Log.debug("Read %d bytes.\n", size);
        Log.verbose(buf);
        Log.verbose();

        // Walk through the buffer, discarding comments and reading settings.
        char* p = strtok(buf, "\n");
        while(NULL != p) {
          // eliminate comments
          if('*' != p[0]) {
            String entry = p;
            Log.debug("ENTRY: %s\n", entry.c_str());
            // Entries are broken into name and value by the = sign.
            int pos = entry.indexOf('=');
            if(-1 == pos) {
              Log.error("Config error: no '=' found in %s\n", entry.c_str());
            }
            else {
              String name = entry.substring(0, pos);
              String value = entry.substring(pos+1);
              Log.info("Config: Adding ConfigParam: name=%s, value=%s\n", name.c_str(), value.c_str());
              this->add(new ConfigParam(name.c_str(), value.c_str()));
            }
          }
          p = strtok(NULL, "\n");
        }
        delete [] buf;
        return true;
      }
    }
    else {
      Log.error("Failed to locate config file %s\n", this->configFileName.c_str());
    }
  }
  return false;
}


// Find the property with the name provided.
// If found, return true.  If value is not null,
// set value to the c_str pointer in the string value.
// If not found, return false and do not alter value.
//
bool Config::get(const char* name, const char** value) {
  ConfigParam* params = this->head;
  while(NULL != params) {
    // Log.debug("Config.get: Comparing %s with %s\n", name, params->name.c_str());
    if(0 == strcmp(name, params->name.c_str())) {
      if(NULL != value) {
        *value = params->value.c_str();
      }
      return true;
    }
    params = params->next;
  }
  return false;
}
