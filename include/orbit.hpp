#ifndef ORBIT_HPP
#define ORBIT_HPP

#include "env.hpp"
#include "delay.hpp"
#include "router.hpp"
#include "request.hpp"
#include <vector>
#include <algorithm>
class Orbit
{
    std::vector<Request *> requests;
    Delay *delay;
    Router *orbitChannel;
    Router *orbitAppendChannel;

public:
    Orbit(Delay *delay, Router *orbitChannel, Router *orbitAppendChannel)
    {
        this->delay = delay;
        this->orbitChannel = orbitChannel;
        this->orbitAppendChannel = orbitAppendChannel;
    }
    void Append()
    {
        for (int i = 0; i < orbitAppendChannel->Len(); i++)
        {
            Request *req = orbitAppendChannel->Pop();
            req->StatusChangeAt = delay->Get();
            EventQueue.push_back(req->StatusChangeAt);
            req->Status = statusTravel;
            requests.push_back(req);
        }
    }
    void Produce()
    {
        auto iterator = std::find_if(requests.begin(), requests.end(), [&](const Request *elem)
                                     { return elem->StatusChangeAt == Time; });
        if (iterator != requests.end())
        {
            orbitChannel->Push(*iterator);
            requests.erase(iterator);
        }
    }
};

#endif