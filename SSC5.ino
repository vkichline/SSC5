//  New SSC5 design, intended for Wemos (LOLIN) D1 Mini
//  Use WiFiManager to connect to network.
//  Use Params.dat in SPIFFS for configuration
//  Van Kichline
//  August 2019

#include  "SensorCluster.h"

SensorCluster*  cluster;


void setup() {
  cluster = new SensorCluster();
  cluster->init();
}

void loop() {
  cluster->loop();
}
