//  Passive Infrared Sensor
//  Implemented with Wemos PIR Shield
//  PIR data is on D3

#include "Sensor.h"

class PirSensor : Sensor {  
  public:
    PirSensor(AppConfig& configParam, MQTT& mqttParam, Log& logParam, const char* nameParam) :
      Sensor(configParam, mqttParam, logParam, nameParam) {}

    bool init() {
      pinMode(PirPin, INPUT);  // Wemos PIR shield, standarddigial input for rev 1.0.0
      logger.verbose("Initialized PirSensor\n");
      return true;
    }

    void begin() {
      logger.verbose("PirSensor::begin()\n");
      this->count = 0;
      this->val   = 0;
    }

    void detect() {
      if(HIGH == digitalRead(PirPin)) {
        val++;
      }
      count++;
      logger.verbose("Sensor: %s - count = %d, val = %d\n", this->name, this->count, this->val);
    }

    void report() {
      if(0 < this->count) {
        // Report the average PIR on time as %
        float fPir = (float)(this->val) * 100.0 / (float)(this->count);
        logger.info("PIR: %3.2f%%\n", fPir);
        mqtt.publishFloat(this->feedName, fPir, 1);
      }
    }

    private:
      const int PirPin  = D3;
      int       count   = 0;
      int       val     = 0;
};
