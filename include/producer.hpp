#ifndef PRODUCER_HPP
#define PRODUCER_HPP
#include "router.hpp"
#include "string"
#include "vector"
class Producer
{
public:
    virtual ~Producer() {}
    virtual void Produce() = 0;
    virtual std::string Tag() { return "base"; }
    virtual Slot *SlotAt(std::string slot_name) = 0;
    virtual std::vector<std::string> GetSlotNames() = 0;
};

#endif