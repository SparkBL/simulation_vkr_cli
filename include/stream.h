#ifndef STREAM_H
#define STREAM_H
#include "event.h"
#include "utils.h"
#include "processable.h"
#include <vector>

class SimpleInputProcess : public Processable
{
protected:
    Event *nextEvent;
    double intensity;
    EventType claimsType;

public:
    SimpleInputProcess(DelayProvider *d, double intensity, EventType claimsType);
    Event *Process();
};

class MMPPInputProcess : public Processable
{
protected:
    std::vector<std::vector<double>> shiftProbs;
    std::vector<double> intensities;
    int matrixSize;
    Event *shiftMoment;
    int currentState;
    Event *nextEvent;
    EventType claimsType;
    int Shift();

public:
    MMPPInputProcess(DelayProvider *d, std::vector<std::vector<double>> shiftProbs, std::vector<double> intensities, int matrixSize, EventType claimsType);
    Event *Process();
    const int *GetStatePointer();
};

#endif // STREAM_H
