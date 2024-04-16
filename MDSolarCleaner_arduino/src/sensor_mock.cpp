#include <Arduino.h>
#include <SPI.h>
#include "MDSolarCleaner_functions.h"

float distance_up = 1.0;
float distance_down = 1.0; 
float distance_left = 1.0; 
float distance_right = 1.0;
float temperature = 21.0;
float surface_temp = 21.0;
float humidity = 55.0;
float brush1_speed = 200;
float brush2_speed = 200;
float flow_velocity = 10;
float acceleration = 0.5;

int air_dust_density = 50;

void setup(){
    Serial.begin(9600);
    randomSeed(7524890);
};

void loop(){
    send_data_to_raspy(distance_up, distance_down, distance_right, 
                       distance_left, temperature, surface_temp,
                       humidity, air_dust_density, brush1_speed, 
                       brush2_speed, flow_velocity, acceleration
                       );
    distance_up         += random(-100, 101)/3000.0;
    distance_down       += random(-100, 101)/3000.0; 
    distance_right      += random(-100, 101)/3000.0; 
    distance_left       += random(-100, 101)/3000.0; 
    temperature         += random(-100, 101)/1000.0;
    surface_temp        += random(-100, 101)/1000.0;
    humidity            += random(-100, 101)/100.0;
    air_dust_density    += random(-2, 3);
    brush1_speed        += random(-100, 101)/10.0;
    brush2_speed        += random(-100, 101)/10.0;
    flow_velocity       += random(-100, 101)/100.0; 
    acceleration        += random(-100, 101)/1000.0; 
    delay(100);     // to be replaced by HW-Timer triggered Interrupt
};