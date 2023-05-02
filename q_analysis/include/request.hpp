#ifndef REQUEST_HPP
#define REQUEST_HPP

static unsigned int sNextId = 0;
unsigned int getNextId() { return ++sNextId; }

// namespace enums
//{
const int typeInput = 0;
const int typeCalled = 1;
const int typeState = 2;

const int statusTravel = 0;
const int statusServing = 1;
const int statusServed = 2;
const int statusLeave = 3;
const int statusArrive = 4;
//};
struct Request
{
    unsigned int id = getNextId();
    int rtype;
    int status;
    double emitted_at;
    double wait_time = 0;
    double status_change_at;
};

struct IntervalStat
{
    int input;
    int called;
    int state;
};

#endif