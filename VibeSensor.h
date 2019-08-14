//  Sensor to calculate vibration using a custom GY-521
//  Gyro and Accellerometer module.
//

#include "Sensor.h"
#include "Wire.h"


class VibeSensor : public Sensor {  
  public:
    VibeSensor(AppConfig& configParam, MQTT& mqttParam, Log& logParam, const char* nameParam) :
      Sensor(configParam, mqttParam, logParam, nameParam) {}

    bool init() {
      logger.verbose("Initialized VibeSensor\n");
      Wire.begin();
      Wire.beginTransmission(MPU_addr);
      Wire.write(0x6B);  // PWR_MGMT_1 register
      Wire.write(0);     // set to zero (wakes up the MPU-6050)
      Wire.endTransmission(true);
      return true;
    }

    void begin() {
      logger.verbose("VibeSensor::begin()\n");
      count = 0;
      val   = 0.0;
      last_x = last_y = last_z = 0;
    }

    void read() {
      int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

      Wire.beginTransmission(MPU_addr);
      Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
      Wire.endTransmission(false);
      Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
      AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
      AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
      AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
      Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
      GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
      GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
      GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
      logger.verbose("AcX = %i", AcX);
      logger.verbose(" | AcY = %i", AcY);
      logger.verbose(" | AcZ = %i", AcZ);
      logger.verbose(" | Tmp = %.1f", Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
      logger.verbose(" | GyX = %i", GyX);
      logger.verbose(" | GyY = %i", GyY);
      logger.verbose(" | GyZ = %i\n", GyZ);
      if(!((0.0 == last_x) && (0.0 == last_y) && (0.0 == last_z))) {
        int16_t diff_x = abs(last_x - AcX);
        int16_t diff_y = abs(last_y - AcY);
        int16_t diff_z = abs(last_z - AcZ);
        if(diff_x < threshold) diff_x = 0;
        if(diff_y < threshold) diff_y = 0;
        if(diff_z < threshold) diff_z = 0;
        logger.verbose("diff_x = %i, diff_y = %i, diff_z = %i\n", diff_x, diff_y, diff_z);
        val += sqrt(diff_x + diff_y + diff_z);  // Accumulate the sum or roots, so small vibrations stand out.
        logger.debug("Sensor: %s - count = %d, val = %f\n", name, count, val);
      }
      last_x = AcX;
      last_y = AcY;
      last_z = AcZ;

      count++;
      logger.verbose("Sensor: %s - count = %d, val = %f\n", name, count, val);
    }

    void report() {
      if(0 < count) {
        // Report the average VCC over time
        val /= (float)count;
        logger.info("Vibe: %3.2f\n", val);
        mqtt.publishFloat(feedName, val, 1);
      }
    }

    private:
      const int     MPU_addr  = 0x68; // I2C address of the MPU-6050
      const int16_t threshold = 325;  // Noise elimination
      int           count     = 0;
      float         val       = 0.0;
      int16_t       last_x    = 0;
      int16_t       last_y    = 0;
      int16_t       last_z    = 0;
};
