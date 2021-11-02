#ifndef REQUEST_HPP
#define REQUEST_HPP
const int TypeInput = 0;
const int TypeCalled = 1;

const int statusTravel = 0;
const int statusServing = 1;
const int statusServed = 2;

struct Request
{
    int Type;
    int Status;
    double StatusChangeAt;
};

#endif