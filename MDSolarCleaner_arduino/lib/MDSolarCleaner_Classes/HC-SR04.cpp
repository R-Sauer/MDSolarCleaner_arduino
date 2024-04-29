#include "HC-SR04.h"

HCSR04::HCSR04(uint16_t trigger, uint8_t echo)
{
    triggPin = trigger;
    echoPin = echo;

    pinMode(triggPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

float HCSR04::readDistance()
{
    // Sende ein Ultraschall-Puls-Signal aus
    digitalWrite(triggPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggPin, LOW);

    // Berechne die Entfernung basierend auf der gemessenen Zeit und der Schallgeschwindigkeit
    dist = (pulseIn(echoPin, HIGH) / 2) / 29.1;  // Formel für die Berechnung der Entfernung in Zentimetern
    return dist;
}
