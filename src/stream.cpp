#include "stream.h"




SimpleInputProcess::SimpleInputProcess(DelayProvider* d,double intensity,EventType claimsType) :Processable(d) {
    this->intensity = intensity;
    this->claimsType = claimsType;
    nextEvent = new Event(d->GetDelay(intensity),claimsType);
}

Event* SimpleInputProcess::Process(){
    Event *ret = nullptr;
    if (nextEvent->Moment() == Environment::Time())
    {
        ret = nextEvent;
        nextEvent = new Event(d->GetDelay(intensity),claimsType);
    }
    return ret;
}




MMPPInputProcess::MMPPInputProcess(DelayProvider* d, std::vector<std::vector<double>> shiftProbs,std::vector<double> intensities, int matrixSize,EventType claimsType)
    : Processable(d){
    this->shiftProbs = shiftProbs;
    this->intensities = intensities;
    this->matrixSize = matrixSize;
    this->claimsType = claimsType;
    currentState = 0;
    shiftMoment = new Event( d->GetDelay(- shiftProbs[currentState][currentState]),EventType::Shift);
    nextEvent = new Event(d->GetDelay(intensities[currentState]),claimsType);
}

Event* MMPPInputProcess::Process()
{
    Shift();
    Event *ret = nullptr;
    if (nextEvent->Moment() == Environment::Time())
    {
        ret = nextEvent;
        nextEvent = new Event(d->GetDelay(intensities[currentState]),claimsType);
    }
    return ret;
}

int MMPPInputProcess::Shift()
{
    if (shiftMoment->Moment() ==Environment::Time())
    {
        double sum = 0;
        double chance = utils::NextDouble();
        for(int i =0; i<matrixSize;i++){
            if (i!=currentState){
                sum+= shiftProbs[currentState][i]/(-shiftProbs[currentState][currentState]);
                if(chance<=sum){
                    currentState = i;
                    nextEvent = new Event(d->GetDelay(intensities[currentState]),claimsType);
                    shiftMoment = new Event(d->GetDelay(-shiftProbs[currentState][currentState]),EventType::Shift);
                    return currentState;
                }
            }
        }
    }
    return -1;
}

const int* MMPPInputProcess::GetStatePointer(){
    return &currentState;
}
