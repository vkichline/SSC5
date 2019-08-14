//  Create an array of Sensors, based on Config settings.
//  Distribute begin, read and report over the full list.
//  Config will have a list of sensor enable settings like:
//    VccSensor=true
//    PirSensor=true
//    LuxSensor=true
//    TempSensor=true
//    HumSensor=true
//    BaroSensor=false
//    VibeSensor=true


#include "Sensor.h"
#include  "Log.h"       // For reporting status and issues to output
#include  "MQTT.h"      // For reporting values to message broker
#include  "AppConfig.h" // Needed to construct feed name, probably other things as well.


class SensorList {
  public:
    SensorList(AppConfig* configParam, MQTT& mqttParam, Log& logParam);

    bool        init();    // Called from setup. Returns true if all inits succeed.
    void        begin();   // Called at beginning of aquisition loop.
    void        read();    // Called repeatedly durring aquisition.
    void        report();  // Called at end of acquisition loop. Reports data to mqtt.

  protected:
    Sensor**    sensors;
    int         numSensors;
    AppConfig*  config;
    MQTT&       mqtt;
    Log&        logger;
};
