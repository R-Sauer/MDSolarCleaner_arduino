#include "TLE4935L.h"

TLE4935L::TLE4935L(uint8_t _qPin, uint8_t _edgesPerCycle, float _gearRatio)
{
    pinMode(_qPin, INPUT);
    this->qPin = _qPin;
    this->edgeCounter = 0;
    this->lastRPM = 0;
    this->microsOfLastCalcRPM = micros();
    this->edgesPerCycle = _edgesPerCycle;
    this->gearRatio = _gearRatio;
}

void TLE4935L::incrementEdgeCounter()
{
    this->edgeCounter++;
}

float TLE4935L::getLastRPM()
{
    return this->lastRPM;
}

uint8_t TLE4935L::getSensorPin()
{
    return this->qPin;
}

float TLE4935L::calcRPM()
{
    uint32_t edgeCounterTemp;
    unsigned long now = micros();
    unsigned long interval = now - this->microsOfLastCalcRPM;
    this->microsOfLastCalcRPM = now;
    noInterrupts();
    edgeCounterTemp = this->edgeCounter;
    edgeCounter = 0;
    interrupts();
    float edgesPerMin = static_cast<float>(60000000.0/interval * edgeCounterTemp);
    this->lastRPM = float((edgesPerMin/this->edgesPerCycle)*gearRatio);
    return lastRPM;
}