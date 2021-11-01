#ifndef NODE_H
#define NODE_H
#include "event.h"
#include "processable.h"

class INode : public Processable
{
public:
    INode(DelayProvider *d);
    virtual Event *Income(Event *income) = 0;
    virtual ~INode(){};
};

class CalledClaimsNode : public INode
{
protected:
    double serveIntensity;
    double calledServeIntensity;
    Event *currentEvent;

public:
    CalledClaimsNode(DelayProvider *d, double serve_intensity, double called_serve_intensity);
    Event *Process();
    Event *Income(Event *income);
    bool IsFree();
};

#endif // NODE_H
