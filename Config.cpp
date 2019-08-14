#include <FS.h>
#include "Config.h"
#include "Log.h"

extern Log Log;


Config::Config(const char* pConfigFileName) {
  configFileName = new char[strlen(pConfigFileName)+1];
  strcpy(configFileName, pConfigFileName);
  configured = false;
}

Config::Config() {
  configFileName = "/Config.dat";
  configured = false;
}


void Config::add(ConfigParam* param) {
  // If the list is empty, this becomes the head and the tail:
  if(NULL == tail) {
    head = tail = param;
  }
  // Otherwise, link param to the existing tail, and it becomes the tail.
  else {
    tail->link(param);
    tail = param;
  }
}

//  Return true if Config.dat was read successfully
//  false if not.
bool Config::init() {
  if(configured) {
    Log.error("Config Error: attempt to re-initialize.\n");
    return false;
  }
  if (SPIFFS.begin()) {
    Log.debug("Config: mounted file system.\n");
    if (SPIFFS.exists(configFileName)) {
      Log.debug("Reading %s\n", configFileName);
      File configFile = SPIFFS.open(configFileName, "r");
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
            // At this point, p points to a cstr that should be Name=Value
            Log.debug("ENTRY: %s\n", p);
            // Entries are broken into name and value by the = sign, but strtok cannot be nested.
            char* pos = strchr(p, '=');
            if(NULL == pos) {
              Log.error("Config error: no '=' found in %s\n", p);
            }
            else {
              // Drop a NULL where the = sign was found.
              *pos = 0;
              const char* name  = p;
              const char* value = ++pos;
              Log.info("Config: Adding ConfigParam: name=%s, value=%s\n", name, value);
              add(new ConfigParam(name, value));
            }
          }
          p = strtok(NULL, "\n");
        }
        // NOTE: do NOT delete buffer!  // delete [] buf;
        return true;
      }
    }
    else {
      Log.error("Failed to locate config file %s\n", configFileName);
    }
  }
  return false;
}


// Find the property with the name provided.
// If found, return true.  If value is not NULL,
// set value const char*.
// If not found, return false and do not alter value.
//
bool Config::get(const char* name, const char** value) {
  ConfigParam* params = head;
  while(NULL != params) {
    // Log.debug("Config.get: Comparing %s with %s\n", name, params->name);
    if(0 == strcmp(name, params->name)) {
      if(NULL != value) {
        *value = params->value;
      }
      return true;
    }
    params = params->next;
  }
  return false;
}
