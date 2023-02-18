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
    Request now_serving_;
    Delay *input_delay_;
    Delay *called_delay_;
    InSlot in_channel_;
    InSlot call_channel_;
    InSlot orbit_channel_;
    OutSlot orbit_append_channel_;
    OutSlot out_channel_;

public:
    RQTNode(Delay *input_delay,
            Delay *called_delay)
    {
        this->input_delay_ = input_delay;
        this->called_delay_ = called_delay;
        this->now_serving_ = Request{status : statusServed};
        inputs_ = {{"in_slot", &in_channel_}, {"call_slot", &call_channel_}, {"orbit_slot", &orbit_channel_}};
        outputs_ = {{"orbit_append_slot", &orbit_append_channel_}, {"out_slot", &out_channel_}};
    }

    std::vector<double> Produce(double time) override
    {
        if (now_serving_.status == statusServing && now_serving_.status_change_at == time)
        {
            now_serving_.status = statusServed;
            out_channel_.Push(now_serving_);
        }
        if (!in_channel_.IsEmpty())
        {
            if (now_serving_.status == statusServing)
            {
                orbit_append_channel_.Push(in_channel_.Pop());
            }
            else
            {
                now_serving_ = in_channel_.Pop();
                now_serving_.wait_time = time - now_serving_.emitted_at;
                now_serving_.status_change_at = input_delay_->Get(time);
                now_serving_.status = statusServing;
                queue.push_back(now_serving_.status_change_at);
            }
        }

        if (!orbit_channel_.IsEmpty())
        {
            if (now_serving_.status == statusServing)
            {
                orbit_append_channel_.Push(orbit_channel_.Pop());
            }
            else
            {
                now_serving_ = orbit_channel_.Pop();
                now_serving_.wait_time = time - now_serving_.emitted_at;
                now_serving_.status_change_at = input_delay_->Get(time);
                now_serving_.status = statusServing;
                queue.push_back(now_serving_.status_change_at);
            }
        }

        if (!call_channel_.IsEmpty())
        {
            if (now_serving_.status != statusServing)
            {
                now_serving_ = call_channel_.Pop();
                now_serving_.status_change_at = called_delay_->Get(time);
                now_serving_.status = statusServing;
                queue.push_back(now_serving_.status_change_at);
            }
            else
            {
                call_channel_.Pop();
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
    Request now_serving_;
    Delay *delay_;
    InSlot in_channel_;
    OutSlot out_channel_;

public:
    SimpleNode(Delay *delay)
    {
        this->delay_ = delay;
        this->now_serving_ = Request{status : statusServed};
        inputs_ = {{"in_slot", &in_channel_}};
        outputs_ = {{"out_slot", &out_channel_}};
    }

    std::vector<double> Produce(double time) override
    {
        if (now_serving_.status == statusServing && now_serving_.status_change_at == time)
        {
            now_serving_.status = statusServed;
            out_channel_.Push(now_serving_);
        }
        if (!in_channel_.IsEmpty())
        {
            if (now_serving_.status != statusServing)
            {
                now_serving_ = in_channel_.Pop();
                now_serving_.wait_time = time - now_serving_.emitted_at;
                now_serving_.status_change_at = delay_->Get(time);
                now_serving_.status = statusServing;
                queue.push_back(now_serving_.status_change_at);
            }
        }
        return GetEvents();
    }

    std::string Tag() override { return "simple_node"; }
};

class RQNode : public Producer
{
    Request now_serving_;
    Delay *delay_;
    InSlot in_channel_;
    InSlot orbit_channel_;
    OutSlot orbit_append_channel_;
    OutSlot out_channel_;

public:
    RQNode(Delay *delay)
    {
        this->delay_ = delay;
        this->now_serving_ = Request{status : statusServed};
        inputs_ = {{"in_slot", &in_channel_}, {"orbit_slot", &orbit_channel_}};
        outputs_ = {{"orbit_append_slot", &orbit_append_channel_}, {"out_slot", &out_channel_}};
    }

    std::vector<double> Produce(double time) override
    {
        if (now_serving_.status == statusServing && now_serving_.status_change_at == time)
        {
            now_serving_.status = statusServed;
            out_channel_.Push(now_serving_);
        }
        if (!in_channel_.IsEmpty())
        {
            if (now_serving_.status == statusServing)
            {
                orbit_append_channel_.Push(in_channel_.Pop());
            }
            else
            {
                now_serving_ = in_channel_.Pop();
                now_serving_.wait_time = time - now_serving_.emitted_at;
                now_serving_.status_change_at = delay_->Get(time);
                now_serving_.status = statusServing;
                queue.push_back(now_serving_.status_change_at);
            }
        }

        if (!orbit_channel_.IsEmpty())
        {
            if (now_serving_.status == statusServing)
            {
                orbit_append_channel_.Push(orbit_channel_.Pop());
            }
            else
            {
                now_serving_ = orbit_channel_.Pop();
                now_serving_.wait_time = time - now_serving_.emitted_at;
                now_serving_.status_change_at = delay_->Get(time);
                now_serving_.status = statusServing;
                queue.push_back(now_serving_.status_change_at);
            }
        }
        return GetEvents();
    }

    std::string Tag() override { return "rq_node"; }
};

//Прибор с поломкой
class RQTPollingNode : public Producer
{
    Request now_serving_;
    Delay *input_delay_;
    Delay *called_delay_;
    InSlot in_channel_;
    InSlot call_channel_;
    InSlot orbit_channel_;
    OutSlot orbit_append_channel_;
    OutSlot out_channel_;

public:
    RQTPollingNode(Delay *input_delay,
                   Delay *called_delay)
    {
        this->input_delay_ = input_delay;
        this->called_delay_ = called_delay;
        this->now_serving_ = Request{status : statusServed};
        inputs_ = {{"in_slot", &in_channel_}, {"call_slot", &call_channel_}, {"orbit_slot", &orbit_channel_}};
        outputs_ = {{"orbit_append_slot", &orbit_append_channel_}, {"out_slot", &out_channel_}};
    }

    std::vector<double> Produce(double time) override
    {
        if (now_serving_.status == statusServing && now_serving_.status_change_at == time)
        {
            now_serving_.status = statusServed;
            out_channel_.Push(now_serving_);
        }
        if (!in_channel_.IsEmpty())
        {
            if (now_serving_.status == statusServing)
            {
                orbit_append_channel_.Push(in_channel_.Pop());
            }
            else
            {
                now_serving_ = in_channel_.Pop();
                now_serving_.wait_time = time - now_serving_.emitted_at;
                now_serving_.status_change_at = input_delay_->Get(time);
                now_serving_.status = statusServing;
                queue.push_back(now_serving_.status_change_at);
            }
        }

        if (!orbit_channel_.IsEmpty())
        {
            if (now_serving_.status == statusServing)
            {
                orbit_append_channel_.Push(orbit_channel_.Pop());
            }
            else
            {
                now_serving_ = orbit_channel_.Pop();
                now_serving_.wait_time = time - now_serving_.emitted_at;
                now_serving_.status_change_at = input_delay_->Get(time);
                now_serving_.status = statusServing;
                queue.push_back(now_serving_.status_change_at);
            }
        }

        if (!call_channel_.IsEmpty())
        {
            if (now_serving_.status != statusServing)
            {
                now_serving_ = call_channel_.Pop();
                now_serving_.status_change_at = called_delay_->Get(time);
                now_serving_.status = statusServing;
                queue.push_back(now_serving_.status_change_at);
            }
            else
            {
                call_channel_.Pop();
            }
        }
        return GetEvents();
    }

    std::string Tag() override
    {
        return "rqt_polling_node";
    }
};

//Прибор с покиданием заявок
/*
class LeaveNode{

}
*/
#endif
