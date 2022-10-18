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
    std::vector<std::vector<double>> q_;
    std::vector<double> l_;
    int request_type_;
    int state_;
    double shift_time_;
    Request next_produce_;
    OutSlot channel_;

    void shift(double time)
    {
        if (shift_time_ == time)
        {
            double sum = 0;
            double chance = NextDouble();
            for (int i = 0; i < q_.size(); i++)
            {
                if (i != state_)
                {
                    sum += q_[state_][i] / (-q_[state_][state_]);
                    if (chance <= sum)
                    {
                        state_ = i;
                        next_produce_ = Request{rtype : request_type_, status : statusTravel, status_change_at : GetExponentialDelay(l_[state_], time)};
                        shift_time_ = GetExponentialDelay(-q_[state_][state_], time);
                        queue.push_back(next_produce_.status_change_at);
                        queue.push_back(shift_time_);
                        return;
                    }
                }
            }
        }
    }

public:
    MMPP(std::vector<double> l, std::vector<std::vector<double>> q, int request_type, double init_time = 0)
    {
        this->request_type_ = request_type;

        this->l_ = l;
        this->q_ = q;
        outputs_ = {{"out_slot", &channel_}};
        inputs_ = {};
        double t = GetExponentialDelay(l_[0], init_time);
        next_produce_ = Request{rtype : request_type_, status : statusTravel, emitted_at : t, status_change_at : t};
        queue.push_back(next_produce_.status_change_at);
        state_ = 0;
        shift_time_ = GetExponentialDelay(-q_[0][0], init_time);
        queue.push_back(shift_time_);
    }

    std::vector<double> Produce(double time) override
    {
        shift(time);
        if (next_produce_.status_change_at == time)
        {
            channel_.Push(next_produce_);
            double t = GetExponentialDelay(l_[state_], time);
            next_produce_ = Request{rtype : request_type_, status : statusTravel, emitted_at : t, status_change_at : t};
            if (next_produce_.status_change_at < shift_time_)
            {
                queue.push_back(next_produce_.status_change_at);
            }
        }
        return GetEvents();
    }

    std::string Tag() override { return "mmpp_input"; }
};

class SimpleInput : public Producer
{
    Request next_produce_;
    Delay *delay_;
    int request_type_;
    OutSlot channel_;

public:
    SimpleInput(Delay *delay, int request_type, double init_time = 0)
    {
        this->delay_ = delay;
        this->request_type_ = request_type;
        inputs_ = {};
        outputs_ = {{"out_slot", &channel_}};
        double t = delay_->Get(init_time);
        next_produce_ = Request{
            rtype : request_type_,
            status : statusTravel,
            emitted_at : t,
            status_change_at : t
        };
        queue.push_back(next_produce_.status_change_at);
    }

    std::vector<double> Produce(double time) override
    {
        if (next_produce_.status_change_at == time)
        {
            channel_.Push(next_produce_);
            double t = delay_->Get(time);
            next_produce_ = Request{rtype : request_type_, status : statusTravel, emitted_at : t, status_change_at : t};
            queue.push_back(next_produce_.status_change_at);
        }
        return GetEvents();
    }
    std::string Tag() override { return "simple_input"; }
};
//;
#endif