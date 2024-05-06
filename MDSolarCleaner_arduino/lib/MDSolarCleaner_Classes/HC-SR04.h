#include <Arduino.h>

#ifndef HCSR04_H
#define HCSR04_H

#define US_TIMEOUT 4000
#define US_CYCLETIME 4250

class HCSR04 {
    public:
        HCSR04(uint16_t _trigPin, uint8_t _echoPin, uint32_t _pulseTimeout = US_TIMEOUT, uint32_t _cycleTimeUs = US_CYCLETIME);
        float readDistance();
    private:
        uint8_t echoPin;
        uint8_t triggPin;
        uint32_t pulseTimeout;
        uint32_t cycleTimeUs;
        float dist;
};

#endif