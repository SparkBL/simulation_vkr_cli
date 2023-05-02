#ifndef NODE_HPP
#define NODE_HPP

#include "delay.hpp"
#include "model.hpp"
#include "producer.hpp"
#include "router.hpp"
#include "request.hpp"
#include <unordered_map>
#include <vector>
#include <utils.hpp>
// namespace nodes
//{

class DumpNode : public Producer
{
    Request next_produce;
    int request_type;
    const std::vector<std::string> in_slot_names = {"in_slot"};
    const std::vector<std::string> out_slot_names = {};

public:
    DumpNode()
    {
        for (auto &s : out_slot_names)
        {
            outputs.insert({s, OutSlot()});
        }
        for (auto &s : in_slot_names)
        {
            inputs.insert({s, InSlot()});
        }
    }

    std::vector<double> Produce(double time) override
    {
        inputs[in_slot_names[0]].Pop();
        return GetEvents();
    }
    std::string Tag() override { return "dump_node"; }
};

class RQTNode : public Producer
{
    Request now_serving;
    Delay &input_delay;
    Delay &called_delay;
    const std::vector<std::string> in_slot_names = {"in_slot", "orbit_slot", "call_slot"};
    const std::vector<std::string> out_slot_names = {"out_slot", "orbit_append_slot"};

public:
    RQTNode(Delay &input_delay,
            Delay &called_delay) : input_delay(input_delay), called_delay(called_delay)
    {
        this->now_serving = Request{status : statusServed};
        for (auto &s : out_slot_names)
        {
            outputs.insert({s, OutSlot()});
        }
        for (auto &s : in_slot_names)
        {
            inputs.insert({s, InSlot()});
        }
    }

    std::vector<double> Produce(double time) override
    {
        if (now_serving.status == statusServing && now_serving.status_change_at == time)
        {
            now_serving.status = statusServed;
            outputs[out_slot_names[0]].Push(now_serving);
        }
        if (!inputs[in_slot_names[0]].IsEmpty())
        {
            if (now_serving.status == statusServing)
            {
                outputs[out_slot_names[1]].Push(inputs[in_slot_names[0]].Pop());
            }
            else
            {
                now_serving = inputs[in_slot_names[0]].Pop();
                now_serving.wait_time = time - now_serving.emitted_at;
                now_serving.status_change_at = input_delay.Get(time);
                now_serving.status = statusServing;
                queue.push_back(now_serving.status_change_at);
            }
        }

        if (!inputs[in_slot_names[1]].IsEmpty())
        {
            if (now_serving.status == statusServing)
            {
                outputs[out_slot_names[1]].Push(inputs[in_slot_names[1]].Pop());
            }
            else
            {
                now_serving = inputs[in_slot_names[1]].Pop();
                now_serving.wait_time = time - now_serving.emitted_at;
                now_serving.status_change_at = input_delay.Get(time);
                now_serving.status = statusServing;
                queue.push_back(now_serving.status_change_at);
            }
        }

        if (!inputs[in_slot_names[2]].IsEmpty())
        {
            if (now_serving.status != statusServing)
            {
                now_serving = inputs[in_slot_names[2]].Pop();
                now_serving.status_change_at = called_delay.Get(time);
                now_serving.status = statusServing;
                queue.push_back(now_serving.status_change_at);
            }
            else
            {
                inputs[in_slot_names[2]].Pop();
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
    Delay &delay;
    const std::vector<std::string> in_slot_names = {"in_slot"};
    const std::vector<std::string> out_slot_names = {"out_slot"};

public:
    SimpleNode(Delay &delay) : delay(delay)
    {
        this->now_serving = Request{status : statusServed};
        for (auto &s : out_slot_names)
        {
            outputs.insert({s, OutSlot()});
        }
        for (auto &s : in_slot_names)
        {
            inputs.insert({s, InSlot()});
        }
    }

    std::vector<double> Produce(double time) override
    {
        if (now_serving.status == statusServing && now_serving.status_change_at == time)
        {
            now_serving.status = statusServed;
            outputs[out_slot_names[0]].Push(now_serving);
        }
        if (!inputs[in_slot_names[0]].IsEmpty())
        {
            if (now_serving.status != statusServing)
            {
                now_serving = inputs[in_slot_names[0]].Pop();
                now_serving.wait_time = time - now_serving.emitted_at;
                now_serving.status_change_at = delay.Get(time);
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
    Delay &delay;
    const std::vector<std::string> in_slot_names = {"in_slot", "orbit_slot"};
    const std::vector<std::string> out_slot_names = {"out_slot", "orbit_append_slot"};

public:
    RQNode(Delay &delay) : delay(delay)
    {
        this->now_serving = Request{status : statusServed};
        for (auto &s : out_slot_names)
        {
            outputs.insert({s, OutSlot()});
        }
        for (auto &s : in_slot_names)
        {
            inputs.insert({s, InSlot()});
        }
    }

    std::vector<double> Produce(double time) override
    {
        if (now_serving.status == statusServing && now_serving.status_change_at == time)
        {
            now_serving.status = statusServed;
            outputs[out_slot_names[0]].Push(now_serving);
        }
        if (!inputs[in_slot_names[0]].IsEmpty())
        {
            if (now_serving.status == statusServing)
            {
                outputs[out_slot_names[1]].Push(inputs[in_slot_names[0]].Pop());
            }
            else
            {
                now_serving = inputs[in_slot_names[0]].Pop();
                now_serving.wait_time = time - now_serving.emitted_at;
                now_serving.status_change_at = delay.Get(time);
                now_serving.status = statusServing;
                queue.push_back(now_serving.status_change_at);
            }
        }

        if (!inputs[in_slot_names[1]].IsEmpty())
        {
            if (now_serving.status == statusServing)
            {
                outputs[out_slot_names[1]].Push(inputs[in_slot_names[1]].Pop());
            }
            else
            {
                now_serving = inputs[in_slot_names[1]].Pop();
                now_serving.wait_time = time - now_serving.emitted_at;
                now_serving.status_change_at = delay.Get(time);
                now_serving.status = statusServing;
                queue.push_back(now_serving.status_change_at);
            }
        }
        return GetEvents();
    }

    std::string Tag() override { return "rq_node"; }
};
/*
// Прибор с поломкой
class RQTPollingNode : public Producer
{
    Request now_serving;
    Delay &input_delay;
    Delay &called_delay;
    // Delay *poll_delay;
    InSlot in_channel;
    InSlot call_channel;
    InSlot orbit_channel;
    OutSlot orbit_append_channel;
    OutSlot out_channel;
    std::vector<std::vector<double>> q;
    double poll_time;
    int state;

private:
    void poll(double time)
    {
        if (poll_time == time)
        {
            double sum = 0;
            double chance = NextDouble();
            for (int i = 0; i < q.size(); i++)
            {
                if (i != state)
                {
                    sum += q[state][i] / (-q[state][state]);
                    if (chance <= sum)
                    {
                        state = i;
                        poll_time = GetExponentialDelay(-q[state][state], time);
                        queue.push_back(poll_time);
                        return;
                    }
                }
            }
        }
    }

public:
    RQTPollingNode(Delay *input_delay,
                   Delay *called_delay, std::vector<std::vector<double>> q)
    {
        if (q.size() != 3)
        {
            throw std::invalid_argument("q must be of size 3");
        }
        if (q.at(0).size() != 3 || q.at(1).size() != 3 || q.at(2).size() != 3)
        {
            throw std::invalid_argument("q must be of size 3");
        }
        if (q[0][1] != 0 || q[1][0] != 0)
        {
            throw std::invalid_argument("q[0][1] and q[1][0] must equal 0");
        }
        this->q = q;
        this->input_delay = input_delay;
        this->called_delay = called_delay;
        this->now_serving = Request{status : statusServed};
        inputs = {{slot_names[0], in_channel}, {"call_slot", call_channel}, {"orbit_slot", orbit_channel}};
        outputs = {{"orbit_append_slot", orbit_append_channel}, {"out_slot", out_channel}};
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
};*/

// Прибор с покиданием заявок
/*
class LeaveNode{

}
*/
#endif
