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
public:
    std::unordered_map<double, Request> requests;
    Delay &delay;
    const std::vector<std::string> in_slot_names = {"in_slot"};
    const std::vector<std::string> out_slot_names = {"out_slot"};

    Orbit(Delay &delay) : delay(delay)
    {
        requests = {};
        for (auto &s : out_slot_names)
        {
            outputs.insert({s, OutSlot()});
        }
        for (auto &s : in_slot_names)
        {
            inputs.insert({s, InSlot()});
        }

        // Description gen
        std::ostringstream ss;
        ss << "Orbit " << std::endl
           << "Delays:" << std::endl
           << "# Return Delay:" << std::endl
           << delay.Describe() << std::endl;
        description = ss.str();
    }

    std::vector<double> Append(double time) override
    {
        while (!inputs[in_slot_names[0]].IsEmpty())
        {
            Request req = inputs[in_slot_names[0]].Pop();
            req.status_change_at = delay.Get(time);
            queue.push_back(req.status_change_at);
            requests.insert(std::make_pair(req.status_change_at, req));
            // requests.push_back(req);
        }
        return GetEvents();
    }
    std::vector<double> Produce(double time) override
    {
        /*  for (std::vector<Request>::size_type i = 0; i != requests.size(); i++)
          {
              if (requests[i].status_change_at == time)
              {
                  outputs[out_slot_names[0]].Push(requests[i]);
                  requests.erase(requests.begin() + i);
                  return std::vector<double>();
              }
          }*/
        if (requests.find(time) != requests.end())
        {
            outputs[out_slot_names[0]].Push(requests[time]);
            requests.erase(time);
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