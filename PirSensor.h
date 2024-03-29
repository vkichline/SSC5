//  Passive Infrared Sensor
//  Implemented with Wemos PIR Shield
//  PIR input is defined in Pins.h

#include "Constants.h"
#include "Sensor.h"

class PirSensor : public Sensor {  
  public:
    PirSensor(AppConfig* configParam, MQTT& mqttParam, Log& logParam, const char* nameParam) :
      Sensor(configParam, mqttParam, logParam, nameParam) {}

    bool init() {
      pinMode(PirPin, INPUT);  // Wemos PIR shield
      logger.verbose("Initialized PirSensor\n");
      return true;
    }

    void begin() {
      logger.verbose("PirSensor::begin()\n");
      count = 0;
      val   = 0;
    }

    void read() {
      if(HIGH == digitalRead(PirPin)) {
        val++;
      }
      count++;
      logger.debug("Sensor: %s - count = %d, val = %d\n", name, count, val);
    }

    void report() {
      if(0 < count) {
        // Report the average PIR on time as %
        float fPir = (float)val * 100.0 / (float)count;
        logger.info("PIR: %3.2f%%\n", fPir);
        mqtt.publishFloat(feedName, fPir, 1);
      }
    }

    private:
      const int PirPin  = PIR_INPUT_PIN;
      int       count   = 0;
      int       val     = 0;
};
