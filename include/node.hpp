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
            Delay *called_delay)
    {
        this->input_delay_ = input_delay;
        this->called_delay_ = called_delay;
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

    Slot *SlotAt(std::string slot_name) override
    {
        if (slot_name == "in_slot")
            return &in_channel_;
        if (slot_name == "call_slot")
            return &call_channel_;
        if (slot_name == "orbit_slot")
            return &orbit_channel_;
        if (slot_name == "orbit_append_slot")
            return &orbit_append_channel_;
        if (slot_name == "out_slot")
            return &out_channel_;
        return nullptr;
    }

    std::vector<std::string> GetSlotNames() override
    {
        return std::vector<std::string>{"in_slot", "call_slot", "orbit_slot", "orbit_append_slot", "out_slot"};
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
    Slot *SlotAt(std::string slot_name) override
    {
        if (slot_name == "in_slot")
            return &in_channel_;
        if (slot_name == "out_slot")
            return &out_channel_;
        return nullptr;
    }
    std::vector<std::string> GetSlotNames() override
    {
        return std::vector<std::string>{"in_slot", "orbit_slot"};
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
    RQNode(Delay *delay)
    {
        this->delay_ = delay;
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
    Slot *SlotAt(std::string slot_name) override
    {
        if (slot_name == "in_slot")
            return &in_channel_;
        if (slot_name == "orbit_slot")
            return &orbit_channel_;
        if (slot_name == "orbit_append_slot")
            return &orbit_append_channel_;
        if (slot_name == "out_slot")
            return &out_channel_;
        return nullptr;
    }
    std::vector<std::string> GetSlotNames() override
    {
        return std::vector<std::string>{"in_slot", "orbit_slot", "orbit_append_slot", "out_slot"};
    }
};
//};
#endif