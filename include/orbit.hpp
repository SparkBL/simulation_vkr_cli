#ifndef ORBIT_HPP
#define ORBIT_HPP

#include "env.hpp"
#include "delay.hpp"
#include "router.hpp"
#include "request.hpp"
#include <vector>

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
        if (requests.size() > 0)
        {
            for (int i = 0; i < requests.size(); i++)
            {
                if (almostEqual(requests[i]->StatusChangeAt, Time))
                {
                    Request *ret = requests[i];
                    requests.erase(requests.begin() + i);
                    orbitChannel->Push(ret);
                    return;
                }
            }
        }
    }
};

#endif