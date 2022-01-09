#ifndef STREAM_HPP
#define STREAM_HPP

#include <vector>
#include "request.hpp"
#include "router.hpp"
#include "delay.hpp"
#include "producer.hpp"

class MMPP : public Producer
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
                        nextProduce = Request{Type : RequestType, Status : statusTravel, StatusChangeAt : ExponentialDelay(L[state])};
                        shiftTime = ExponentialDelay(-Q[state][state]);
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
        nextProduce = Request{Type : RequestType, Status : statusTravel, StatusChangeAt : ExponentialDelay(L[0])};
        EventQueue.push_back(nextProduce.StatusChangeAt);
        state = 0;
        shiftTime = ExponentialDelay(-Q[0][0]);
        EventQueue.push_back(shiftTime);
    }

    void Produce()
    {
        shift();
        if (nextProduce.StatusChangeAt == Time)
        {
            channel->Push(nextProduce);
            nextProduce = Request{Type : RequestType, Status : statusTravel, StatusChangeAt : ExponentialDelay(L[state])};
            if (nextProduce.StatusChangeAt < shiftTime)
            {
                EventQueue.push_back(nextProduce.StatusChangeAt);
            }
        }
    }
};

class SimpleInput : public Producer
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