#ifndef STREAM_HPP
#define STREAM_HPP

#include <vector>
#include "request.hpp"
#include "router.hpp"
#include "delay.hpp"
#include "producer.hpp"
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

    void shift()
    {
        if (shift_time_ == Time)
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
                        next_produce_ = Request{type : request_type_, status : statusTravel, status_change_at : GetExponentialDelay(l_[state_])};
                        shift_time_ = GetExponentialDelay(-q_[state_][state_]);
                        EventQueue.push_back(next_produce_.status_change_at);
                        EventQueue.push_back(shift_time_);
                        return;
                    }
                }
            }
        }
    }

public:
    MMPP(std::vector<double> l, std::vector<std::vector<double>> q, int request_type)
    {
        this->request_type_ = request_type;
        this->l_ = l;
        this->q_ = q;
        next_produce_ = Request{type : request_type_, status : statusTravel, status_change_at : GetExponentialDelay(l_[0])};
        EventQueue.push_back(next_produce_.status_change_at);
        state_ = 0;
        shift_time_ = GetExponentialDelay(-q_[0][0]);
        EventQueue.push_back(shift_time_);
    }

    void Produce() override
    {
        shift();
        if (next_produce_.status_change_at == Time)
        {
            channel_.Push(next_produce_);
            next_produce_ = Request{type : request_type_, status : statusTravel, status_change_at : GetExponentialDelay(l_[state_])};
            if (next_produce_.status_change_at < shift_time_)
            {
                EventQueue.push_back(next_produce_.status_change_at);
            }
        }
    }

    Slot *SlotAt(std::string slot_name) override
    {
        if (slot_name == "out_slot")
            return &channel_;
        return nullptr;
    }

    std::vector<std::string> GetSlotNames() override
    {
        return std::vector<std::string>{"out_slot"};
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
    SimpleInput(Delay *delay, int request_type)
    {
        this->delay_ = delay;
        this->request_type_ = request_type;
        next_produce_ = Request{
            type : request_type_,
            status : statusTravel,
            status_change_at : delay_->Get()
        };
        EventQueue.push_back(next_produce_.status_change_at);
    }

    void Produce() override
    {
        if (next_produce_.status_change_at == Time)
        {
            channel_.Push(next_produce_);
            next_produce_ = Request{type : request_type_, status : statusTravel, status_change_at : delay_->Get()};
            EventQueue.push_back(next_produce_.status_change_at);
        }
    }
    Slot *SlotAt(std::string slot_name) override
    {
        if (slot_name == "out_slot")
            return &channel_;
        return nullptr;
    }

    std::vector<std::string> GetSlotNames() override
    {
        return std::vector<std::string>{"out_slot"};
    }
    std::string Tag() override { return "simple_input"; }
};
//;
#endif