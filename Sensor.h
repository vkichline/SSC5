//  Base class for all sensors

#include  "Log.h"   // For reporting status and issues to output
#include  "MQTT.h"  // For reporting values to message broker

class Sensor {
  public:
    // Initialize the sensor with mqtt provider, log object and sensor name.
    Sensor(MQTT* pMqtt, Log* pLog, const char* name){
      this->pMqtt = pMqtt;
      this->pLog  = pLog;
      this->name = name;
    }
    virtual bool init() { return true; }    // Called once, in setup. Return success of initialization
    
    virtual void begin() {}                 // Called at beginning of aquisition loop.
    virtual void detectAndCollect() {}      // Called repeadedly durring aquisition.
    virtual void report() {}      

  private:
    MQTT*       pMqtt;
    Log*        pLog;
    const char* name;
};
