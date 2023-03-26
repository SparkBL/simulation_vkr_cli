#ifndef NODE_HPP
#define NODE_HPP

#include "delay.hpp"
#include "model.hpp"
#include "producer.hpp"
#include "router.hpp"
#include "request.hpp"
#include <unordered_map>
#include <utils.hpp>
// namespace nodes
//{
class RQTNode : public Producer
{
    Request now_serving;
    Delay *input_delay;
    Delay *called_delay;
    InSlot in_channel;
    InSlot call_channel;
    InSlot orbit_channel;
    OutSlot orbit_append_channel;
    OutSlot out_channel;

public:
    RQTNode(Delay *input_delay,
            Delay *called_delay)
    {
        if (input_delay == nullptr || called_delay == nullptr)
        {
            throw std::invalid_argument("delay object is nil");
        }
        this->input_delay = input_delay;
        this->called_delay = called_delay;
        this->now_serving = Request{status : statusServed};
        inputs = {{"in_slot", &in_channel}, {"call_slot", &call_channel}, {"orbit_slot", &orbit_channel}};
        outputs = {{"orbit_append_slot", &orbit_append_channel}, {"out_slot", &out_channel}};
    }

    std::vector<double> Produce(double time) override
    {
        if (now_serving.status == statusServing && now_serving.status_change_at == time)
        {
            now_serving.status = statusServed;
            out_channel.Push(now_serving);
        }
        if (!in_channel.IsEmpty())
        {
            if (now_serving.status == statusServing)
            {
                orbit_append_channel.Push(in_channel.Pop());
            }
            else
            {
                now_serving = in_channel.Pop();
                now_serving.wait_time = time - now_serving.emitted_at;
                now_serving.status_change_at = input_delay->Get(time);
                now_serving.status = statusServing;
                queue.push_back(now_serving.status_change_at);
            }
        }

        if (!orbit_channel.IsEmpty())
        {
            if (now_serving.status == statusServing)
            {
                orbit_append_channel.Push(orbit_channel.Pop());
            }
            else
            {
                now_serving = orbit_channel.Pop();
                now_serving.wait_time = time - now_serving.emitted_at;
                now_serving.status_change_at = input_delay->Get(time);
                now_serving.status = statusServing;
                queue.push_back(now_serving.status_change_at);
            }
        }

        if (!call_channel.IsEmpty())
        {
            if (now_serving.status != statusServing)
            {
                now_serving = call_channel.Pop();
                now_serving.status_change_at = called_delay->Get(time);
                now_serving.status = statusServing;
                queue.push_back(now_serving.status_change_at);
            }
            else
            {
                call_channel.Pop();
            }
        }
        return GetEvents();
    }

    std::string Tag() override
    {
        return "rqt_node";
    }
};

class SimpleNode : public Producer
{
    Request now_serving;
    Delay *delay;
    InSlot in_channel;
    OutSlot out_channel;

public:
    SimpleNode(Delay *delay)
    {
        if (delay == nullptr)
        {
            throw std::invalid_argument("delay object is nil");
        }
        this->delay = delay;
        this->now_serving = Request{status : statusServed};
        inputs = {{"in_slot", &in_channel}};
        outputs = {{"out_slot", &out_channel}};
    }

    std::vector<double> Produce(double time) override
    {
        if (now_serving.status == statusServing && now_serving.status_change_at == time)
        {
            now_serving.status = statusServed;
            out_channel.Push(now_serving);
        }
        if (!in_channel.IsEmpty())
        {
            if (now_serving.status != statusServing)
            {
                now_serving = in_channel.Pop();
                now_serving.wait_time = time - now_serving.emitted_at;
                now_serving.status_change_at = delay->Get(time);
                now_serving.status = statusServing;
                queue.push_back(now_serving.status_change_at);
            }
        }
        return GetEvents();
    }

    std::string Tag() override { return "simple_node"; }
};

class RQNode : public Producer
{
    Request now_serving;
    Delay *delay;
    InSlot in_channel;
    InSlot orbit_channel;
    OutSlot orbit_append_channel;
    OutSlot out_channel;

public:
    RQNode(Delay *delay)
    {
        if (delay == nullptr)
        {
            throw std::invalid_argument("delay object is nil");
        }
        this->delay = delay;
        this->now_serving = Request{status : statusServed};
        inputs = {{"in_slot", &in_channel}, {"orbit_slot", &orbit_channel}};
        outputs = {{"orbit_append_slot", &orbit_append_channel}, {"out_slot", &out_channel}};
    }

    std::vector<double> Produce(double time) override
    {
        if (now_serving.status == statusServing && now_serving.status_change_at == time)
        {
            now_serving.status = statusServed;
            out_channel.Push(now_serving);
        }
        if (!in_channel.IsEmpty())
        {
            if (now_serving.status == statusServing)
            {
                orbit_append_channel.Push(in_channel.Pop());
            }
            else
            {
                now_serving = in_channel.Pop();
                now_serving.wait_time = time - now_serving.emitted_at;
                now_serving.status_change_at = delay->Get(time);
                now_serving.status = statusServing;
                queue.push_back(now_serving.status_change_at);
            }
        }

        if (!orbit_channel.IsEmpty())
        {
            if (now_serving.status == statusServing)
            {
                orbit_append_channel.Push(orbit_channel.Pop());
            }
            else
            {
                now_serving = orbit_channel.Pop();
                now_serving.wait_time = time - now_serving.emitted_at;
                now_serving.status_change_at = delay->Get(time);
                now_serving.status = statusServing;
                queue.push_back(now_serving.status_change_at);
            }
        }
        return GetEvents();
    }

    std::string Tag() override { return "rq_node"; }
};

// Прибор с поломкой
class RQTPollingNode : public Producer
{
    Request now_serving;
    Delay *input_delay;
    Delay *called_delay;
    InSlot in_channel;
    InSlot call_channel;
    InSlot orbit_channel;
    OutSlot orbit_append_channel;
    OutSlot out_channel;
    std::vector<std::vector<double>> Q;

private:
    void poll()
    {
    }

public:
    RQTPollingNode(Delay *input_delay,
                   Delay *called_delay)
    {
        this->input_delay = input_delay;
        this->called_delay = called_delay;
        this->now_serving = Request{status : statusServed};
        inputs = {{"in_slot", &in_channel}, {"call_slot", &call_channel}, {"orbit_slot", &orbit_channel}};
        outputs = {{"orbit_append_slot", &orbit_append_channel}, {"out_slot", &out_channel}};
    }

    std::vector<double> Produce(double time) override
    {
        if (now_serving.status == statusServing && now_serving.status_change_at == time)
        {
            now_serving.status = statusServed;
            out_channel.Push(now_serving);
        }
        if (!in_channel.IsEmpty())
        {
            if (now_serving.status == statusServing)
            {
                orbit_append_channel.Push(in_channel.Pop());
            }
            else
            {
                now_serving = in_channel.Pop();
                now_serving.wait_time = time - now_serving.emitted_at;
                now_serving.status_change_at = input_delay->Get(time);
                now_serving.status = statusServing;
                queue.push_back(now_serving.status_change_at);
            }
        }

        if (!orbit_channel.IsEmpty())
        {
            if (now_serving.status == statusServing)
            {
                orbit_append_channel.Push(orbit_channel.Pop());
            }
            else
            {
                now_serving = orbit_channel.Pop();
                now_serving.wait_time = time - now_serving.emitted_at;
                now_serving.status_change_at = input_delay->Get(time);
                now_serving.status = statusServing;
                queue.push_back(now_serving.status_change_at);
            }
        }

        if (!call_channel.IsEmpty())
        {
            if (now_serving.status != statusServing)
            {
                now_serving = call_channel.Pop();
                now_serving.status_change_at = called_delay->Get(time);
                now_serving.status = statusServing;
                queue.push_back(now_serving.status_change_at);
            }
            else
            {
                call_channel.Pop();
            }
        }
        return GetEvents();
    }

    std::string Tag() override
    {
        return "rqt_polling_node";
    }
};

// Прибор с покиданием заявок
/*
class LeaveNode{

}
*/
#endif
