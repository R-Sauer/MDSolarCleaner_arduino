#include <Arduino.h>
#include <SPI.h>
#include <ACS712.h>
#include "MDSolarCleaner_functions.h"
#include "TLE4935L.h"
#include "pinDefines.h"

#define SERIAL_BAUD 115200
#define SENSOR_DATA_FIELDS 3
#define SENSOR_MSG_FREQUENCY 100
#define SENSOR_MSG_PERIOD_US 1000000/SENSOR_MSG_FREQUENCY

// define ISRs for brush RPM
void countRotation_brush1();
void countRotation_brush2();
ISR(TIMER4_COMPA_vect);

//ACS712-30A Current Sensor
ACS712 acs(CURRENT_SENSOR_PIN, 5.0, 1023, 66);

//TLE4935L Hall Effect Sensors
TLE4935L brush1(RPM_BRUSH1_PIN, 2);
TLE4935L brush2(RPM_BRUSH2_PIN, 2);

// Sensorvariable
float sensorValues[SENSOR_DATA_FIELDS];

// Loop timing variables
unsigned long loopStartUs = 0;
unsigned long loopDurationUs = 0;

void setup()
{

    Serial.begin(SERIAL_BAUD);

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

    // Read momentary sensor values
    sensorValues[0] = brush1.getLastRPM();
    sensorValues[1] = brush2.getLastRPM();
    sensorValues[2] = acs.mA_DC();

    // Write to serial
    writeSensorvaluesToSerial(sensorValues, SENSOR_DATA_FIELDS);

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