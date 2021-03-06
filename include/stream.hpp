#ifndef STREAM_HPP
#define STREAM_HPP

#include <vector>
#include "request.hpp"
#include "router.hpp"
#include "delay.hpp"
#include "producer.hpp"

class IStream
{
public:
    virtual ~IStream(){};
    virtual void Produce() = 0;
};

class MMPP : public IStream
{
    std::vector<std::vector<double>> Q;
    std::vector<double> L;
    int RequestType;
    int state;
    double shiftTime;
    Request nextProduce;
    Router *channel;

    void shift()
    {
        if (shiftTime == Time)
        {
            double sum = 0;
            double chance = NextDouble();
            for (int i = 0; i < Q.size(); i++)
            {
                if (i != state)
                {
                    sum += Q[state][i] / (-Q[state][state]);
                    if (chance <= sum)
                    {
                        state = i;
                        nextProduce = Request{Type : RequestType, Status : statusTravel, StatusChangeAt : GetExponentialDelay(L[state])};
                        shiftTime = GetExponentialDelay(-Q[state][state]);
                        EventQueue.push_back(nextProduce.StatusChangeAt);
                        EventQueue.push_back(shiftTime);
                        return;
                    }
                }
            }
        }
    }

public:
    MMPP(std::vector<double> L, std::vector<std::vector<double>> Q, int RequestType, Router *channel)
    {
        this->RequestType = RequestType;
        this->L = L;
        this->Q = Q;
        this->channel = channel;
        nextProduce = Request{Type : RequestType, Status : statusTravel, StatusChangeAt : GetExponentialDelay(L[0])};
        EventQueue.push_back(nextProduce.StatusChangeAt);
        state = 0;
        shiftTime = GetExponentialDelay(-Q[0][0]);
        EventQueue.push_back(shiftTime);
    }

    void Produce()
    {
        shift();
        if (nextProduce.StatusChangeAt == Time)
        {
            channel->Push(nextProduce);
            nextProduce = Request{Type : RequestType, Status : statusTravel, StatusChangeAt : GetExponentialDelay(L[state])};
            if (nextProduce.StatusChangeAt < shiftTime)
            {
                EventQueue.push_back(nextProduce.StatusChangeAt);
            }
        }
    }
};

class SimpleInput : public IStream
{
    Request nextProduce;
    Delay *delay;
    int RequestType;
    Router *channel;

public:
    SimpleInput(Delay *delay, int RequestType, Router *channel)
    {
        this->delay = delay;
        this->RequestType = RequestType;
        this->channel = channel;
        nextProduce = Request{
            Type : RequestType,
            Status : statusTravel,
            StatusChangeAt : delay->Get()
        };
        EventQueue.push_back(nextProduce.StatusChangeAt);
    }

    void Produce()
    {
        if (nextProduce.StatusChangeAt == Time)
        {
            channel->Push(nextProduce);
            nextProduce = Request{Type : RequestType, Status : statusTravel, StatusChangeAt : delay->Get()};
            EventQueue.push_back(nextProduce.StatusChangeAt);
        }
    }
};

#endif