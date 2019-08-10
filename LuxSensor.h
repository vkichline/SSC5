//  Implemented with Wemos Ambient Light Shield
//

#include "Sensor.h"
#include <BH1750.h>

class LuxSensor : Sensor {
  public:
    LuxSensor(AppConfig& configParam, MQTT& mqttParam, Log& logParam, const char* nameParam) :
      Sensor(configParam, mqttParam, logParam, nameParam) {}

    bool init() {
      sensor = new BH1750(0x23);
      logger.verbose("Initialized LuxSensor\n");
      return true;
    }

    void begin() {
      logger.verbose("LuxSensor::begin()\n");
      count = 0;
      val   = 0;
    }

    void read() {
      if(sensor->begin()) {
        val += sensor->readLightLevel();
      }
      else {
        logger.error("Error reading BH1750 sensor.\n");
      }
      count++;
      logger.verbose("Sensor: %s - count = %d, val = %d\n", name, count, val);
    }

    void report() {
      if(0 < count) {
        // Report the average VCC over time
        val /= count;
        logger.info("Lux: %d\n", val);
        mqtt.publishInt(feedName, val);
      }
    }

  private:
    BH1750* sensor;
    int     count = 0;
    long    val   = 0;
};
