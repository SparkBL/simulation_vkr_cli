#ifndef PROCESSABLE_H
#define PROCESSABLE_H
#include "event.h"
#include "delayprovider.h"

class Processable
{
protected:
    DelayProvider *d;

public:
    Processable();
    Processable(DelayProvider *d);
    virtual Event *Process() = 0;
    virtual ~Processable() {}
};

#endif // PROCESSABLE_H
