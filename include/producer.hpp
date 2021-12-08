#ifndef PRODUCER_HPP
#define PRODUCER_HPP

class Producer
{
public:
    virtual ~Producer(){};
    virtual void Produce() = 0;
};

#endif