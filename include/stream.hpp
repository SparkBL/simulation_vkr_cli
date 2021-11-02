#ifndef STREAM_HPP
#define STREAM_HPP

#include <vector>
#include "request.hpp"
#include "router.hpp"
#include "delay.hpp"
class MMPP
{
    std::vector<std::vector<double>> L, Q;
    int RequestType;
    int state;
    double shiftTime;
    Request nextProduce;
    Router *channel;

    void shift()
    {
        if (almostEqual(shiftTime, Time))
        {
            double sum = 0, chance = NextDouble();
            for (int i = 0; i < Q.size(); i++)
            {
                if (i != state)
                {
                    sum += Q[state][i] / (-Q[state][state]);
                    if (chance <= sum)
                    {
                        state = i;
                        nextProduce = Request{Type : RequestType, Status : statusTravel, StatusChangeAt : ExponentialDelay(L[state][state])};
                        shiftTime = ExponentialDelay(-Q[state][state]);
                        EventQueue.push_back(nextProduce.StatusChangeAt);
                        EventQueue.push_back(shiftTime);
                    }
                }
            }
        }
    }

public:
    MMPP(std::vector<std::vector<double>> L, std::vector<std::vector<double>> Q, const int &RequstType, Router *channel)
    {
        this->RequestType;
        this->L = L;
        this->Q = Q;
        this->channel = channel;
        nextProduce = Request{Type : RequstType, Status : statusTravel, StatusChangeAt : ExponentialDelay(L[0][0])};
        EventQueue.push_back(nextProduce.StatusChangeAt);
        state = 0;
        shiftTime = ExponentialDelay(-Q[0][0]);
    }

    void Produce()
    {
        shift();
        if (almostEqual(nextProduce.StatusChangeAt, Time))
        {
            channel->Push(nextProduce);
            nextProduce = Request{Type : RequestType, Status : statusTravel, StatusChangeAt : ExponentialDelay(L[state][state])};
            EventQueue.push_back(nextProduce.StatusChangeAt);
        }
    }
};

class SimpleInput
{
    Request nextProduce;
    Delay *delay;
    int RequestType;
    Router *channel;

public:
    SimpleInput(Delay *delay, const int &RequestType, Router *channel)
    {
        this->delay = delay;
        this->RequestType = RequestType;
        this->channel = channel;
        nextProduce = Request{
            Type :
                RequestType,
            Status : statusTravel,
            StatusChangeAt : delay->Get()
        };
    }

    void Produce()
    {
        if (almostEqual(nextProduce.StatusChangeAt, Time))
        {
            channel->Push(nextProduce);
            nextProduce = Request{Type : RequestType, Status : statusTravel, StatusChangeAt : delay->Get()};
            EventQueue.push_back(nextProduce.StatusChangeAt);
        }
    }
};

#endif