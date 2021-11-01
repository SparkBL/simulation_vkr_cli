#ifndef ORBIT_H
#define ORBIT_H
#include <vector>
#include "event.h"
#include "processable.h"

class IOrbit : public Processable
{
public:
    IOrbit(DelayProvider *d);
    virtual void Accept(Event *claim) = 0;
    virtual ~IOrbit(){};
};

class Orbit : public IOrbit
{
private:
    std::vector<Event *> claimStorage;
    double delayIntensity, delayIntensity1;

public:
    Orbit(DelayProvider *d, double delayIntensity, double delayIntensity1 = 0);
    Event *Process();
    void Accept(Event *claim);
};

#endif // ORBIT_H
