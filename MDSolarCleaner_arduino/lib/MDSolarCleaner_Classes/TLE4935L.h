// Class to read RPM of a rotating part with "edgesPerCycle" magnets using a TLE4935L IC
#include <Arduino.h>

#ifndef TLE4935L_
#define TLE4935L_

class TLE4935L{
public:
    TLE4935L(uint8_t _qPin, uint8_t _edgesPerCycle, float _gearRatio=1.0);
    void incrementEdgeCounter();
    float calcRPM();
    float getLastRPM();
    uint8_t getSensorPin();
private:
    uint8_t qPin;
    uint16_t edgeCounter;
    unsigned long microsOfLastCalcRPM;
    uint8_t edgesPerCycle;
    float lastRPM;
    float gearRatio;
};

#endif