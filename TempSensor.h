//  Sensor to read WEMOS_SHT3X tempurature/humidity sensor for tempurature

#include "Sensor.h"
#include <WEMOS_SHT3X.h>  // https://github.com/wemos/WEMOS_SHT3x_Arduino_Library


class TempSensor : public Sensor {  
  public:
    TempSensor(AppConfig* configParam, MQTT& mqttParam, Log& logParam, const char* nameParam) :
      Sensor(configParam, mqttParam, logParam, nameParam) {}

    bool init() {
      sensor = new SHT3X(0x45);
      logger.verbose("Initialized TempSensor\n");
      return true;
    }

    void begin() {
      logger.verbose("TempSensor::begin()\n");
      count = 0;
      val   = 0.0;
    }

    void read() {
      if(0 == sensor->get()) {
        count++;
        // Detect and accumulate the tempurature
        val += sensor->fTemp;
        // Detect and accumulate humidity
        //hum += sensor->humidity;
      }
      else {
        logger.error("Error reading SHT30 sensor.\n");
      }
      logger.debug("Sensor: %s - count = %d, val = %f\n", name, count, val);
    }

    void report() {
      if(0 < count) {
        // Report the average VCC over time
        val /= (float)count;
        logger.info("Temp: %3.2f F\n", val);
        mqtt.publishFloat(feedName, val, 1);
      }
    }

    private:
      SHT3X*  sensor;        // Tempurature/humidity shield
      int     count = 0;
      float   val   = 0.0;
};
