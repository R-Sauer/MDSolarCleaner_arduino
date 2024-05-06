#include "HC-SR04.h"

HCSR04::HCSR04(uint16_t _trigPin, uint8_t _echoPin, uint32_t _pulseTimeout, uint32_t _cycleTimeUs)
{
    triggPin = _trigPin;
    echoPin = _echoPin;
    pulseTimeout = _pulseTimeout;
    cycleTimeUs = _cycleTimeUs;

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

    // Nimm die Zeit zum Anfang der Messung
    uint32_t tpin = micros();
    pulseDurationUs = pulseIn(echoPin, HIGH, pulseTimeout);

    // Bestimme die Zeit der Messung in us und warte für die übrige Zykluszeit
    tpin = micros()-tpin;
    delayMicroseconds(cycleTimeUs-tpin);

    // Serial.println(cycleTimeUs-tpin,DEC);

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
