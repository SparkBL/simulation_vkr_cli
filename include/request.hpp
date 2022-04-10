#ifndef REQUEST_HPP
#define REQUEST_HPP
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
    int type;
    int status;
    double status_change_at;
};

#endif