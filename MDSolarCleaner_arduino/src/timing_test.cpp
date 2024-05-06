#include <Arduino.h>
#include <SPI.h>
#include <MPU6050.h>
#include <ACS712.h>
#include "MDSolarCleaner_functions.h"
#include "HC-SR04.h"
#include "pinDefines.h"

#define US_TIMEOUT 4000
#define SERIAL_BAUD 115200

enum SerialMSGMode {teleplot = 0, raspberrypi = 1};

SerialMSGMode serialMSGmode = raspberrypi;

bool serialTimingTest_PI (float distance_up,
                          float distance_down,
                          float distance_right,
                          float distance_left, 
                          // float temperature, 
                          // float surface_temp,
                          // float humidity, 
                          // int air_dust_density, 
                          // float brush1_speed,
                          // float brush2_speed, 
                          // float flow_velocity, 
                          float acceleration,
                          float current) 
{
  String sensorString = String(distance_up) + ";" + 
                        String(distance_down) + ";" +
                        String(distance_right) + ";" + 
                        String(distance_left) + ";" +
                        // String(temperature) + ";" + 
                        // String(surface_temp) + ";" +
                        // String(humidity) + ";" + 
                        // String(air_dust_density) + ";" +
                        // String(brush1_speed) + ";" + 
                        // String(brush2_speed) + ";" +
                        // String(flow_velocity) + ";" + 
                        String(acceleration) + ";" +
                        String(current);

  Serial.println(sensorString);
  return 1;  // Wenn erfolgreich sende 1 sonst 0
}

bool serialTimingTest_TP (float distance_up, 
                          float distance_down, 
                          float distance_right,
                          float distance_left, 
                          // float temperature, 
                          // float surface_temp,
                          // float humidity, 
                          // int air_dust_density, 
                          // float brush1_speed,
                          // float brush2_speed, 
                          // float flow_velocity, 
                          float acceleration,
                          float current) 
{
  Serial.print(">Dist_up:");
  Serial.println(distance_up);
  Serial.print(">Dist_down:");
  Serial.println(distance_down);
  Serial.print(">Dist_right:");
  Serial.println(distance_right);
  Serial.print(">Dist_left:");
  Serial.println(distance_left);
  Serial.print(">Accel_z:");
  Serial.println(acceleration);
  Serial.print(">Current:");
  Serial.println(current);
  return 1;  // Wenn erfolgreich sende 1 sonst 0
}

// Ultrasonicsensors
HCSR04 USdistRight  (US_RIGHT_TRIG, US_RIGHT_ECHO, US_TIMEOUT);
HCSR04 USdistLeft   (US_LEFT_TRIG, US_LEFT_ECHO, US_TIMEOUT);
HCSR04 USdistUp     (US_UP_TRIG, US_UP_ECHO, US_TIMEOUT);
HCSR04 USdistDown   (US_DOWN_TRIG, US_DOWN_ECHO, US_TIMEOUT);

// IMU
MPU6050 mpu;

//ACS712-30A Current Sensor
ACS712 acs(CURRENT_SENSOR_PIN, 5.0, 1023, 66);

// Sensorvariablen
float distance_right = 0.0;         // [cm]
float distance_left = 0.0;          // [cm]
float distance_up = 0.0;            // [cm]
float distance_down = 0.0;          // [cm]
float acceleration = 0.0;           // [g]
float current = 0.0;                // [A]

void setup()
{
    Serial.begin(SERIAL_BAUD);
    
    // Pin für Zeitmessung der Schleife
    pinMode(MEASURE_TIME_PIN, OUTPUT);
    
    // Accelerometer
    Wire.begin();
    mpu.initialize();

    // Current Sensor
    acs.autoMidPoint();
}

void loop()
{
    distance_right  = USdistRight.readDistance();
    distance_left   = USdistLeft.readDistance();
    distance_up     = USdistUp.readDistance();
    distance_down   = USdistDown.readDistance();
    acceleration    = read_acceleration(mpu);
    current         = acs.mA_DC();

    // Set MEASURE_TIME_PIN for duration of serial-transfer
    digitalWrite(MEASURE_TIME_PIN, HIGH);
    switch (serialMSGmode)
    {
      case teleplot:
        serialTimingTest_TP(distance_up, distance_down, distance_right, distance_left, acceleration, current);
      break;
      case raspberrypi:
        serialTimingTest_PI(distance_up, distance_down, distance_right, distance_left, acceleration, current);
      break;
      default:
      break;
    }
    digitalWrite(MEASURE_TIME_PIN, LOW);
}