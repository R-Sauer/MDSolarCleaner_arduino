#include <Arduino.h>

#ifndef HCSR04_H
#define HCSR04_H

class HCSR04 {
    public:
        HCSR04(uint16_t trigger, uint8_t echo);
        float readDistance();
    private:
        uint8_t echoPin;
        uint8_t triggPin;
        float dist;
};

#endif