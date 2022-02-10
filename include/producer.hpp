#ifndef PRODUCER_HPP
#define PRODUCER_HPP
#include "router.hpp"
class Producer
{
public:
    virtual ~Producer(){};
    virtual void Produce() = 0;
    virtual void Connect(const char *slot_name, Router *router) = 0;
};

#endif