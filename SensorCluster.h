//  Base class for the Sensor Cluster, based on
//  Cluster (network and MQTT) and SensorList.

#pragma  once
#include "Cluster.h"
#include "SensorList.h"


class SensorCluster : Cluster {
  public:
    SensorCluster() {}

    bool init() {
      if(Cluster::init()) {
        sensors = new SensorList(config, *mqtt, logger);
        return sensors->init();
      }
      return false;
    }

    void loop() {
      // Calculate how long to keep looping:
      time_t loop_limit = millis() + config->delay;

      // If initialization failed; pause a minute and try again.
      if(!initialized) {
        logger.error("System not initialized/configured.\n");
        delay(INIT_FAILURE_DELAY);
        init();
        return;
      }

      sensors->begin(); // Clear counters and start sampling
      
      while(millis() < loop_limit) {
        mqtt->ensureConnection(); // Reconnect if necessary
        sensors->read();          // Accumulate counts or averages
        delay(500);
      }
      
      sensors->report();          // Send data to MQTT broker
    }

  private:
    SensorList* sensors;
};
