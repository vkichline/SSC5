//  Sensor to read ESP8266 VCC voltage.
//

#include "Sensor.h"

ADC_MODE(ADC_VCC);  // Set up chip to report VCC voltage on AO using ESP.getVcc()

class VccSensor : Sensor {  
  public:
    VccSensor(AppConfig& configParam, MQTT& mqttParam, Log& logParam, const char* nameParam) :
      Sensor(configParam, mqttParam, logParam, nameParam) {}

    bool init() {
      logger.verbose("Initialized VccSensor\n");
      return true;
    }

    void begin() {
      logger.verbose("VccSensor::begin()\n");
      count = 0;
      val   = 0;
    }

    void read() {
      val += (ESP.getVcc() / 1000.0);
      count++;
      logger.verbose("Sensor: %s - count = %d, val = %d\n", name, count, val);
    }

    void report() {
      if(0 < count) {
        // Report the average VCC over time
        val /= (float)count;
        logger.info("Vcc: %3.2f VDC\n", val);
        mqtt.publishFloat(feedName, val, 1);
      }
    }

    private:
      int       count   = 0;
      float     val     = 0;
};
