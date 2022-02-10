#ifndef STREAM_HPP
#define STREAM_HPP

#include <vector>
#include "request.hpp"
#include "router.hpp"
#include "delay.hpp"
#include "producer.hpp"
//namespace streams
//{
class MMPP : public Producer
{
    std::vector<std::vector<double>> Q;
    std::vector<double> L;
    int RequestType;
    int state;
    double shiftTime;
    Request nextProduce;
    OutSlot channel;

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
                        nextProduce = Request{type : RequestType, status : statusTravel, status_change_at : GetExponentialDelay(L[state])};
                        shiftTime = GetExponentialDelay(-Q[state][state]);
                        EventQueue.push_back(nextProduce.status_change_at);
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
        this->channel.Connect(channel);
        nextProduce = Request{type : RequestType, status : statusTravel, status_change_at : GetExponentialDelay(L[0])};
        EventQueue.push_back(nextProduce.status_change_at);
        state = 0;
        shiftTime = GetExponentialDelay(-Q[0][0]);
        EventQueue.push_back(shiftTime);
    }

    void Produce() override
    {
        shift();
        if (nextProduce.status_change_at == Time)
        {
            channel.Push(nextProduce);
            nextProduce = Request{type : RequestType, status : statusTravel, status_change_at : GetExponentialDelay(L[state])};
            if (nextProduce.status_change_at < shiftTime)
            {
                EventQueue.push_back(nextProduce.status_change_at);
            }
        }
    }
};

class SimpleInput : public Producer
{
    Request nextProduce;
    Delay *delay;
    int RequestType;
    OutSlot channel;

public:
    SimpleInput(Delay *delay, int RequestType, Router *channel)
    {
        this->delay = delay;
        this->RequestType = RequestType;
        this->channel.Connect(channel);
        nextProduce = Request{
            type : RequestType,
            status : statusTravel,
            status_change_at : delay->Get()
        };
        EventQueue.push_back(nextProduce.status_change_at);
    }

    void Produce() override
    {
        if (nextProduce.status_change_at == Time)
        {
            channel.Push(nextProduce);
            nextProduce = Request{type : RequestType, status : statusTravel, status_change_at : delay->Get()};
            EventQueue.push_back(nextProduce.status_change_at);
        }
    }
};
//;
#endif