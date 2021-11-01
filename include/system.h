#ifndef SYSTEM_H
#define SYSTEM_H
#include "event.h"
#include "node.h"
#include "stream.h"
#include "orbit.h"
#include "statistic.h"
#include "processable.h"

class RQSystem : public Processable
{
    INode *node;
    Processable *inputProcess;
    Processable *calledInputProcess;
    IOrbit *orbit;
    Statistic statistic;

    Event *ProcessNonCalledClaim(Event *sended);
    void ProcessCalledClaim();
    void ProcessOutputClaim();

public:
    RQSystem(Processable *input, Processable *calledInput, IOrbit *orbit, INode *node);
    Event *Process();
    const Statistic *GetStatistics() const;
    ~RQSystem();
};

#endif // SYSTEM_H
