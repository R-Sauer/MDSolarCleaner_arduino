#include <Arduino.h>
#include <SPI.h>
#include "MDSolarCleaner_functions.h"

#define SERIAL_BAUD 115200
#define SENSOR_DATA_FIELDS 3
#define SENSOR_MSG_FREQUENCY 100
#define SENSOR_MSG_PERIOD_US 1000000/SENSOR_MSG_FREQUENCY

// Sensorvariable
float sensorValues[SENSOR_DATA_FIELDS] = {0., 0., 0.};

// Loop timing variables
unsigned long loopStartUs = 0;
unsigned long loopDurationUs = 0;

// sawtooth counter
int counter = 0;

void setup(){
    Serial.begin(115200);
    randomSeed(7524890);
};

void loop(){
    
    // Takt current Time
    loopStartUs = micros();

    // Generate "sawtooth" shaped sensor data
    if(counter < 500)
    {
        sensorValues[0] += 1.;
        sensorValues[1] += 1.;
        sensorValues[2] += 1.;
        counter++;
    }
    else
    {
        sensorValues[0] = 0.;
        sensorValues[1] = 0.;
        sensorValues[2] = 0.;
        counter = 0;
    }
    // Write to serial
    writeSensorvaluesToSerial(sensorValues, SENSOR_DATA_FIELDS);

    // Loop timing
    loopDurationUs = micros()-loopStartUs;
    delayMicroseconds(SENSOR_MSG_PERIOD_US-loopDurationUs);
};