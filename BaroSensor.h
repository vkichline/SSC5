//  Sensor to read barometer sensor in InHg
// Using Wemos Barometric Pressure shield

#include "Sensor.h"
#include <LOLIN_HP303B.h>


class BaroSensor : public Sensor {
  public:
    BaroSensor(AppConfig& configParam, MQTT& mqttParam, Log& logParam, const char* nameParam) :
      Sensor(configParam, mqttParam, logParam, nameParam) {}

    bool init() {
      logger.verbose("Initialized BaroSensor\n");
      return true;
    }

    void begin() {
      logger.verbose("BaroSensor::begin()\n");
      HP303BPressureSensor.begin();
      count = 0;
      val   = 0.0;
    }

    void read() {
      int32_t pressure;
      // Detect and accumulate the pressure
      if (0 == HP303BPressureSensor.measurePressureOnce(pressure, oversampling))
      {
        count++;
        val += (float)pressure * 0.0002953;
      }
      else {
        logger.error("Error reading SHT30 sensor.\n");
      }
      logger.debug("Sensor: %s - count = %d, val = %f\n", name, count, val);
    }

    void report() {
      if(0 < count) {
        // Report the average barometric pressure over time
        val /= (float)count;
        logger.info("Pressure: %3.2f InHg\n", val);
        mqtt.publishFloat(feedName, val, 1);
      }
    }

    private:
      LOLIN_HP303B  HP303BPressureSensor; // Wemos Barometric Pressure shield
      const int16_t oversampling  = 7;    // Maximum oversampling.
      int           count         = 0;
      float         val           = 0.0;
};
