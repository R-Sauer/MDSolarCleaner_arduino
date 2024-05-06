#include "HC-SR04.h"

HCSR04::HCSR04(uint16_t _trigPin, uint8_t _echoPin, uint32_t _pulseTimeout)
{
    triggPin = _trigPin;
    echoPin = _echoPin;
    pulseTimeout = _pulseTimeout;

    pinMode(triggPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(triggPin, LOW);
}

float HCSR04::readDistance()
{
    uint32_t pulseDurationUs;
    // Sende ein Ultraschall-Puls-Signal aus
    digitalWrite(triggPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggPin, LOW);

    pulseDurationUs = pulseIn(echoPin, HIGH, pulseTimeout);

    if(pulseDurationUs)
    {
        // Berechne die Entfernung basierend auf der gemessenen Zeit und der Schallgeschwindigkeit
        dist = (float)(pulseDurationUs / 2) / 29.1;  // Formel für die Berechnung der Entfernung in Zentimetern
        return dist;
    }
    else
    {
        // return -1 falls der timeout getriggert wurde
        return -1.;
    }
}
