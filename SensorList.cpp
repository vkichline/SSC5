#include "SensorList.h"
#include "AppConfig.h"
#include "VccSensor.h"
#include "PirSensor.h"
#include "LuxSensor.h"
#include "TempSensor.h"
#include "HumSensor.h"

// Create the array of Sensors.
//
SensorList::SensorList(AppConfig& configParam, MQTT& mqttParam, Log& logParam) :
  config(configParam), mqtt(mqttParam), logger(logParam) {
  // Determine how large to make the sensor array:
  numSensors = 0;
  if(config.enableVccSensor)  numSensors++;
  if(config.enablePirSensor)  numSensors++;
  if(config.enableLuxSensor)  numSensors++;
  if(config.enableTempSensor) numSensors++;
  if(config.enableHumSensor)  numSensors++;
  //if(config.enableBaroSensor) numSensors++;
  //if(config.enableVibeSensor) numSensors++;
  logger.verbose("Creating an array of %d sensors.\n", numSensors);
  sensors = new Sensor*[numSensors];

  // Fill in the list.
  int i = 0;
  if(config.enableVccSensor)  sensors[i++] = new VccSensor(configParam, mqttParam, logParam, "vcc");
  if(config.enablePirSensor)  sensors[i++] = new PirSensor(configParam, mqttParam, logParam, "pir");
  if(config.enableLuxSensor)  sensors[i++] = new LuxSensor(configParam, mqttParam, logParam, "lux");
  if(config.enableTempSensor) sensors[i++] = new TempSensor(configParam, mqttParam, logParam, "temp");
  if(config.enableHumSensor)  sensors[i++] = new HumSensor(configParam, mqttParam, logParam, "hum");
  //if(config.enableBaroSensor) sensors[i++] = new BaroSensor(configParam, mqttParam, logParam, "baro");
  //if(config.enableVibeSensor) sensors[i++] = new VibeSensor(configParam, mqttParam, logParam, "vibe");
}

bool SensorList::init() {
  bool result = true;
  logger.verbose("SensorList::init: Itterating over %d items.\n", numSensors);
  for(int i = 0; i < numSensors; i++) {
    if(!sensors[i]->init()) {
      result = false;
    }
  }
  return result;
}

void SensorList::begin() {
  logger.verbose("SensorList::begin: Itterating over %d items.\n", numSensors);
  for(int i = 0; i < numSensors; i++) {
    sensors[i]->begin();
  }
}

void SensorList::read() {
  logger.verbose("SensorList::read: Itterating over %d items.\n", numSensors);
  for(int i = 0; i < numSensors; i++) {
    sensors[i]->read();
  }
}

void SensorList::report() {
  logger.verbose("SensorList::report: Itterating over %d items.\n", numSensors);
  for(int i = 0; i < numSensors; i++) {
    sensors[i]->report();
  }
}
