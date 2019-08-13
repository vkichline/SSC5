//  Doppler Microwave Radar Sensor
//  Implemented with RCWL-0516
//  Radar input is defined in Pins.h

#include "Pins.h"
#include "Sensor.h"

class RadarSensor : public Sensor {  
  public:
    RadarSensor(AppConfig& configParam, MQTT& mqttParam, Log& logParam, const char* nameParam) :
      Sensor(configParam, mqttParam, logParam, nameParam) {}

    bool init() {
      pinMode(RadarPin, INPUT);  // Custom proto shield
      logger.verbose("Initialized RadarSensor\n");
      return true;
    }

    void begin() {
      logger.verbose("RadarSensor::begin()\n");
      count = 0;
      val   = 0;
    }

    void read() {
      if(HIGH == digitalRead(RadarPin)) {
        val++;
      }
      count++;
      logger.debug("Sensor: %s - count = %d, val = %d\n", name, count, val);
    }

    void report() {
      if(0 < count) {
        // Report the average Radar on time as %
        float fRadar = (float)val * 100.0 / (float)count;
        logger.info("Radar: %3.2f%%\n", fRadar);
        mqtt.publishFloat(feedName, fRadar, 1);
      }
    }

    private:
      const int RadarPin = RADAR_INPUT_PIN;
      int       count    = 0;
      int       val      = 0;
};
