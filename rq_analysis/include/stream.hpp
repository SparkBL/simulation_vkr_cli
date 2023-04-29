#ifndef STREAM_HPP
#define STREAM_HPP

#include <vector>
#include "request.hpp"
#include "router.hpp"
#include "delay.hpp"
#include "producer.hpp"
#include "utils.hpp"
#include <unordered_map>
// namespace streams
//{
class MMPP : public Producer
{
    std::vector<std::vector<double>> q;
    std::vector<double> l;
    int request_type;
    int state;
    double shift_time;
    Request next_produce;
    const std::vector<std::string> in_slot_names = {};
    const std::vector<std::string> out_slot_names = {"out_slot"};

    void shift(double time)
    {
        if (shift_time == time)
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
                        next_produce = Request{rtype : request_type, status : statusTravel, emitted_at : time, status_change_at : GetExponentialDelay(l[state], time)};
                        shift_time = GetExponentialDelay(-q[state][state], time);
                        queue.push_back(next_produce.status_change_at);
                        queue.push_back(shift_time);
                        return;
                    }
                }
            }
        }
    }

public:
    MMPP(std::vector<double> l, std::vector<std::vector<double>> q, int request_type, double init_time = 0)
    {
        this->request_type = request_type;

        this->l = l;
        this->q = q;
        for (auto &s : out_slot_names)
        {
            outputs.insert({s, OutSlot()});
        }

        double t = GetExponentialDelay(l[0], init_time);
        next_produce = Request{rtype : request_type, status : statusTravel, emitted_at : t, status_change_at : t};
        queue.push_back(next_produce.status_change_at);
        state = 0;
        shift_time = GetExponentialDelay(-q[0][0], init_time);
        queue.push_back(shift_time);
    }

    std::vector<double> Produce(double time) override
    {
        shift(time);
        if (next_produce.status_change_at == time)
        {
            outputs.at(out_slot_names[0]).Push(next_produce);
            double t = GetExponentialDelay(l[state], time);
            next_produce = Request{rtype : request_type, status : statusTravel, emitted_at : t, status_change_at : t};
            if (next_produce.status_change_at < shift_time)
            {
                queue.push_back(next_produce.status_change_at);
            }
        }
        return GetEvents();
    }

    std::string Tag() override { return "mmpp_input"; }
};

class SimpleInput : public Producer
{
    Request next_produce;
    Delay &delay;
    int request_type;
    const std::vector<std::string> in_slot_names = {};
    const std::vector<std::string> out_slot_names = {"out_slot"};

public:
    SimpleInput(Delay &delay, int request_type, double init_time = 0) : delay(delay)
    {
        this->request_type = request_type;
        for (auto &s : out_slot_names)
        {
            outputs.insert({s, OutSlot()});
        }
        double t = delay.Get(init_time);
        next_produce = Request{
            rtype : request_type,
            status : statusTravel,
            emitted_at : t,
            status_change_at : t
        };
        queue.push_back(next_produce.status_change_at);
    }

    std::vector<double> Produce(double time) override
    {
        if (next_produce.status_change_at == time)
        {

            //   std::cout << outputs.at(out_slot_names[0]).Len() << std::endl;

            outputs.at(out_slot_names[0]).Push(next_produce);
            double t = delay.Get(time);
            next_produce = Request{rtype : request_type, status : statusTravel, emitted_at : t, status_change_at : t};
            queue.push_back(next_produce.status_change_at);
        }
        return GetEvents();
    }
    std::string Tag() override { return "simple_input"; }
};
//;
#endif