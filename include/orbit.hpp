#ifndef ORBIT_HPP
#define ORBIT_HPP

#include "env.hpp"
#include "producer.hpp"
#include "delay.hpp"
#include "router.hpp"
#include "request.hpp"
#include <vector>
#include <algorithm>

class IOrbit : public Producer
{
public:
    virtual void Append() = 0;
};

class Orbit : public Producer
{
protected:
    std::vector<Request> requests;
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
        while (!orbitAppendChannel->IsEmpty())
        {
            Request req = orbitAppendChannel->Pop();
            req.StatusChangeAt = delay->Get();
            EventQueue.push_back(req.StatusChangeAt);
            requests.push_back(req);
        }
    }
    void Produce()
    {
        for (std::vector<Request>::size_type i = 0; i != requests.size(); i++)
        {
            if (requests[i].StatusChangeAt == Time)
            {
                orbitChannel->Push(requests[i]);
                requests.erase(requests.begin() + i);
                return;
            }
        }
    }
};

class StateOrbit : public Orbit
{
    Router *stateChannel;

public:
    StateOrbit(Delay *delay, Router *orbitChannel, Router *orbitAppendChannel, Router *stateChannel) : Orbit(delay, orbitChannel, orbitAppendChannel)
    {
        this->stateChannel = stateChannel;
    }
    void Append()
    {
        while (!orbitAppendChannel->IsEmpty())
        {
            Request req = orbitAppendChannel->Pop();
            stateChannel->Push(Request{
                Type : TypeState,
                Status : statusArrive,
                StatusChangeAt : req.StatusChangeAt,
            });
            req.StatusChangeAt = delay->Get();
            EventQueue.push_back(req.StatusChangeAt);
            requests.push_back(req);
        }
    }
    void Produce()
    {
        for (std::vector<Request>::size_type i = 0; i != requests.size(); i++)
        {
            if (requests[i].StatusChangeAt == Time)
            {
                orbitChannel->Push(requests[i]);
                requests.erase(requests.begin() + i);
                stateChannel->Push(Request{Type : TypeState, Status : statusLeave, StatusChangeAt : Time});
                return;
            }
        }
    }
};

#endif