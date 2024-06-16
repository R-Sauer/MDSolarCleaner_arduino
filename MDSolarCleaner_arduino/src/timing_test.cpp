/*
author: Raphael Sauer
mailto: raphael.sauer@haw-hamburg.de
date:   06/2024

This Program serves as a test, to measure the percentage of the cycle time 
the Arduino spends sending data to the serial interface with an oscilloscope on pin MEASURE_TIME_PIN.
*/

#include <Arduino.h>
#include <SPI.h>
#include <ACS712.h>
#include "MDSolarCleaner_functions.h"
#include "TLE4935L.h"
#include "pinDefines.h"

#define SERIAL_BAUD 115200
#define SENSOR_MSG_FREQUENCY 100
#define SENSOR_MSG_PERIOD_US 1000000/SENSOR_MSG_FREQUENCY * 1000000

enum SerialMSGMode {teleplot = 0, raspberrypi = 1};

SerialMSGMode serialMSGmode = raspberrypi;

// define ISRs for brush RPM
void countRotation_brush1();
void countRotation_brush2();
ISR(TIMER4_COMPA_vect);

void serialTimingTest_PI (float brush1_speed,
                          float brush2_speed, 
                          float current) 
{
  String sensorString = 
                        String(brush1_speed) + ";" + 
                        String(brush2_speed) + ";" +
                        String(current);
  Serial.println(sensorString);
}

void serialTimingTest_TP (float brush1_speed,
                          float brush2_speed, 
                          float current) 
{
  Serial.print(">Brush1_RPM:");
  Serial.println(brush1_speed);
  Serial.print(">Brush2_RPM:");
  Serial.println(brush2_speed);
  Serial.print(">Current:");
  Serial.println(current);
}

//ACS712-30A Current Sensor
ACS712 acs(CURRENT_SENSOR_PIN, 5.0, 1023, 66);

//TLE4935L Hall Effect Sensors
TLE4935L brush1(RPM_BRUSH1_PIN, 2);
TLE4935L brush2(RPM_BRUSH2_PIN, 2);

// Sensorvariablen
float brush1_RPM = 0.0;             // [RPM]
float brush2_RPM = 0.0;             // [RPM]
float current = 0.0;                // [A]

// Loop timing variables
unsigned long loopStartUs = 0;
unsigned long loopDurationUs = 0;

void setup()
{

    Serial.begin(SERIAL_BAUD);
    
    // Pin für Zeitmessung der Schleife
    pinMode(MEASURE_TIME_PIN, OUTPUT);

    // Current Sensor
    acs.autoMidPoint();

    // Set up Interrupts for brush RPM
    attachInterrupt(digitalPinToInterrupt(brush1.getSensorPin()), countRotation_brush1, CHANGE);
    attachInterrupt(digitalPinToInterrupt(brush2.getSensorPin()), countRotation_brush2, CHANGE);

    // // Setting up HW-Timer4 for cyclic RPM calculation
    // Disable Interrupts for setup
    cli();
    //set timer4 interrupt at 0.25Hz
    TCCR4A = 0;// set entire TCCR1A register to 0
    TCCR4B = 0;// same for TCCR1B
    TCNT4  = 0;//initialize counter value to 0
    // set compare match register for 1hz increments
    OCR4A = 3905;// = (16*10^6) / (1*4096) - 1 (must be <65536)
    // turn on CTC mode
    TCCR4B |= (1 << WGM12);
    // Set CS12 and CS10 bits for 1024 prescaler
    TCCR4B |= (1 << CS12) | (1 << CS10);  
    // enable timer compare interrupt
    TIMSK4 |= (1 << OCIE4A);
    // Reenable Interrupts
    sei();
}

void loop()
{
    // Takt current Time
    loopStartUs = micros();

    current = acs.mA_DC();
    brush1_RPM = brush1.getLastRPM();
    brush2_RPM = brush2.getLastRPM();

    // Set MEASURE_TIME_PIN for duration of serial-transfer
    digitalWrite(MEASURE_TIME_PIN, HIGH);
    // Send data over serial interface
    switch (serialMSGmode)
    {
      case teleplot:
        serialTimingTest_TP(brush1_RPM, brush2_RPM, current);
      break;
      case raspberrypi:
        serialTimingTest_PI(brush1_RPM, brush2_RPM, current);
      break;
      default:
      break;
    }
    digitalWrite(MEASURE_TIME_PIN, LOW);

    // Loop timing
    loopDurationUs = micros()-loopStartUs;
    delayMicroseconds(SENSOR_MSG_PERIOD_US-loopDurationUs);
}

void countRotation_brush1() 
{
  brush1.incrementEdgeCounter();
}

void countRotation_brush2() 
{
  brush2.incrementEdgeCounter();
}

ISR(TIMER4_COMPA_vect)
{
  brush1.calcRPM();
  brush2.calcRPM();
};