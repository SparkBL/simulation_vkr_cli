#ifndef NODE_HPP
#define NODE_HPP

#include "delay.hpp"
#include "env.hpp"
#include "producer.hpp"
#include "router.hpp"
#include "request.hpp"
//namespace nodes
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
            Delay *called_delay,
            Router *in_channel,
            Router *call_channel,
            Router *orbit_channel,
            Router *orbit_append_channel,
            Router *out_channel)
    {
        this->input_delay_ = input_delay;
        this->called_delay_ = called_delay;
        this->in_channel_.Connect(in_channel);
        this->call_channel_.Connect(call_channel);
        this->orbit_channel_.Connect(orbit_channel);
        this->orbit_append_channel_.Connect(orbit_append_channel);
        this->out_channel_.Connect(out_channel);
        this->now_serving_ = Request{status : statusServed};
    }

    void Produce() override
    {
        if (now_serving_.status == statusServing && now_serving_.status_change_at == Time)
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
                now_serving_.status_change_at = input_delay_->Get();
                now_serving_.status = statusServing;
                EventQueue.push_back(now_serving_.status_change_at);
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
                now_serving_.status_change_at = input_delay_->Get();
                now_serving_.status = statusServing;
                EventQueue.push_back(now_serving_.status_change_at);
            }
        }

        if (!call_channel_.IsEmpty())
        {
            if (now_serving_.status != statusServing)
            {
                now_serving_ = call_channel_.Pop();
                now_serving_.status_change_at = called_delay_->Get();
                now_serving_.status = statusServing;
                EventQueue.push_back(now_serving_.status_change_at);
            }
            else
            {
                call_channel_.Pop();
            }
        }
    }
};

class SimpleNode : public Producer
{
    Request now_serving_;
    Delay *delay_;
    InSlot in_channel_;
    OutSlot out_channel_;

public:
    SimpleNode(Delay *delay, Router *in_channel, Router *out_channel)
    {
        this->delay_ = delay;
        this->in_channel_.Connect(in_channel);
        this->out_channel_.Connect(out_channel);
        this->now_serving_ = Request{status : statusServed};
    }

    void Produce() override
    {
        if (now_serving_.status == statusServing && now_serving_.status_change_at == Time)
        {
            now_serving_.status = statusServed;
            out_channel_.Push(now_serving_);
        }
        if (!in_channel_.IsEmpty())
        {
            if (now_serving_.status != statusServing)
            {
                now_serving_ = in_channel_.Pop();
                now_serving_.status_change_at = delay_->Get();
                now_serving_.status = statusServing;
                EventQueue.push_back(now_serving_.status_change_at);
            }
        }
    }
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
    RQNode(Delay *delay,
           Router *in_channel,
           Router *orbit_channel,
           Router *orbit_append_channel,
           Router *out_channel)
    {
        this->delay_ = delay;
        this->in_channel_.Connect(in_channel);
        this->orbit_channel_.Connect(orbit_channel);
        this->orbit_append_channel_.Connect(orbit_append_channel);
        this->out_channel_.Connect(out_channel);
        this->now_serving_ = Request{status : statusServed};
    }

    void Produce() override
    {
        if (now_serving_.status == statusServing && now_serving_.status_change_at == Time)
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
                now_serving_.status_change_at = delay_->Get();
                now_serving_.status = statusServing;
                EventQueue.push_back(now_serving_.status_change_at);
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
                now_serving_.status_change_at = delay_->Get();
                now_serving_.status = statusServing;
                EventQueue.push_back(now_serving_.status_change_at);
            }
        }
    }
};
//};
#endif