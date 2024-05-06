#include <Arduino.h>

#ifndef HCSR04_H
#define HCSR04_H

class HCSR04 {
    public:
        HCSR04(uint16_t _trigPin, uint8_t _echoPin, uint32_t _pulseTimeout);
        float readDistance();
    private:
        uint8_t echoPin;
        uint8_t triggPin;
        uint32_t pulseTimeout;
        float dist;
};

#endif