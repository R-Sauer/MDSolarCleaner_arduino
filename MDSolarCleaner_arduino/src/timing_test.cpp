#include <Arduino.h>
#include <SPI.h>
#include "MDSolarCleaner_functions.h"
#include "HC-SR04.h"
#include "pinDefines.h"

bool serialTimingTest    (float distance_up, float distance_down, float distance_right,
                         float distance_left /*, float temperature, float surface_temp,
                         float humidity, int air_dust_density, float brush1_speed,
                         float brush2_speed, float flow_velocity, float acceleration*/) 
{
  String sensorString = String(distance_up) + ";" + String(distance_down) + ";" +
                        String(distance_right) + ";" + String(distance_left);/* + ";" +
                        String(temperature) + ";" + String(surface_temp) + ";" +
                        String(humidity) + ";" + String(air_dust_density) + ";" +
                        String(brush1_speed) + ";" + String(brush2_speed) + ";" +
                        String(flow_velocity) + ";" + String(acceleration);*/

  Serial.println(sensorString);
  return 1;  // Wenn erfolgreich sende 1 sonst 0
}

HCSR04 USdistRight  (US_RIGHT_TRIG, US_RIGHT_ECHO);
HCSR04 USdistLeft   (US_LEFT_TRIG, US_LEFT_ECHO);
HCSR04 USdistUp     (US_UP_TRIG, US_UP_ECHO);
HCSR04 USdistDown   (US_DOWN_TRIG, US_DOWN_ECHO);

// Sensorvariablen
float distance_right = 0.0;         // [cm]
float distance_left = 0.0;          // [cm]
float distance_up = 0.0;            // [cm]
float distance_down = 0.0;          // [cm]

void setup()
{
    Serial.begin(9600);
    pinMode(MEASURE_TIME_PIN, OUTPUT);
}

void loop()
{
    distance_right = USdistRight.readDistance();
    distance_left  = USdistLeft.readDistance();
    distance_up    = USdistUp.readDistance();
    distance_down  = USdistDown.readDistance();

    digitalWrite(MEASURE_TIME_PIN, HIGH);
    serialTimingTest(distance_up, distance_down, distance_right, distance_left);
    digitalWrite(MEASURE_TIME_PIN, LOW);
}