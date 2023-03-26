#ifndef ORBIT_HPP
#define ORBIT_HPP

#include "producer.hpp"
#include "delay.hpp"
#include "router.hpp"
#include "request.hpp"
#include <vector>
#include <algorithm>

class IOrbit : public Producer
{
public:
    virtual std::vector<double> Append(double time) = 0;
};
// namespace orbits
//{
class Orbit : public IOrbit
{
protected:
    std::vector<Request> requests;
    Delay *delay;
    OutSlot orbit_channel;
    InSlot orbit_append_channel;

public:
    Orbit(Delay *delay)
    {
        if (delay == nullptr)
        {
            throw std::invalid_argument("delay object is nil");
        }
        this->delay = delay;
        inputs = {{"in_slot", &orbit_append_channel}};
        outputs = {{"out_slot", &orbit_channel}};
    }

    std::vector<double> Append(double time) override
    {
        while (!orbit_append_channel.IsEmpty())
        {
            Request req = orbit_append_channel.Pop();
            req.status_change_at = delay->Get(time);
            queue.push_back(req.status_change_at);
            requests.push_back(req);
        }
        return GetEvents();
    }
    std::vector<double> Produce(double time) override
    {
        for (std::vector<Request>::size_type i = 0; i != requests.size(); i++)
        {
            if (requests[i].status_change_at == time)
            {
                orbit_channel.Push(requests[i]);
                requests.erase(requests.begin() + i);
                return std::vector<double>();
            }
        }
        return std::vector<double>();
    }

    std::string Tag() override { return "orbit"; }
};

/*class StateOrbit : public Orbit
{
    Router *stateChannel;
public:
    StateOrbit(Delay *delay, Router *orbit_channel, Router *orbit_append_channel, Router *stateChannel) : Orbit(delay, orbit_channel, orbit_append_channel)
    {
        this->stateChannel = stateChannel;
    }
    void Append()
    {
        while (!orbit_append_channel->IsEmpty())
        {
            Request req = orbit_append_channel->Pop();
            stateChannel->Push(Request{
                rtype : typeState,
                status : statusArrive,
                status_change_at : req.status_change_at,
            });
            req.status_change_at = delay->Get();
            EventQueue.push_back(req.status_change_at);
            requests_.push_back(req);
        }
    }
    void Produce()
    {
        for (std::vector<Request>::size_type i = 0; i != requests_.size(); i++)
        {
            if (requests_[i].status_change_at == Time)
            {
                orbit_channel->Push(requests_[i]);
                requests_.erase(requests_.begin() + i);
                stateChannel->Push(Request{type : typeState, status : statusLeave, status_change_at : Time});
                return;
            }
        }
    }
};*/
//};
#endif