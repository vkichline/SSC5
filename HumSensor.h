//  Sensor to read WEMOS_SHT3X tempurature/humidity sensor for humidity

#include "Sensor.h"
#include <WEMOS_SHT3X.h>  // https://github.com/wemos/WEMOS_SHT3x_Arduino_Library


class HumSensor : public Sensor {  
  public:
    HumSensor(AppConfig& configParam, MQTT& mqttParam, Log& logParam, const char* nameParam) :
      Sensor(configParam, mqttParam, logParam, nameParam) {}

    bool init() {
      sensor = new SHT3X(0x45);
      logger.verbose("Initialized HumSensor\n");
      return true;
    }

    void begin() {
      logger.verbose("HumSensor::begin()\n");
      count = 0;
      val   = 0;
    }

    void read() {
      if(0 == sensor->get()) {
        count++;
        // Detect and accumulate humidity
        val += sensor->humidity;
      }
      else {
        logger.error("Error reading SHT30 sensor.\n");
      }
      logger.verbose("Sensor: %s - count = %d, val = %f\n", name, count, val);
    }

    void report() {
      if(0 < count) {
        // Report the average VCC over time
        val /= (float)count;
        logger.info("Hum: %3.2f%% RH\n", val);
        mqtt.publishFloat(feedName, val, 1);
      }
    }

    private:
      SHT3X*  sensor;        // Tempurature/humidity shield
      int     count = 0;
      float   val   = 0;
};
