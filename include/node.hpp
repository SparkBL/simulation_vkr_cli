#ifndef NODE_HPP
#define NODE_HPP

#include "delay.hpp"
#include "env.hpp"
#include "producer.hpp"
#include "router.hpp"
#include "request.hpp"

class RQTNode : public Producer
{
    Request nowServing;
    Delay *inputDelay;
    Delay *calledDelay;
    Router *inChannel;
    Router *callChannel;
    Router *orbitChannel;
    Router *orbitAppendChannel;
    Router *outChannel;

public:
    RQTNode(Delay *inputDelay,
            Delay *calledDelay,
            Router *inChannel,
            Router *callChannel,
            Router *orbitChannel,
            Router *orbitAppendChannel,
            Router *outChannel)
    {
        this->inputDelay = inputDelay;
        this->calledDelay = calledDelay;
        this->inChannel = inChannel;
        this->callChannel = callChannel;
        this->orbitChannel = orbitChannel;
        this->orbitAppendChannel = orbitAppendChannel;
        this->outChannel = outChannel;
        this->nowServing = Request{Status : statusServed};
    }

    void Produce()
    {
        if (nowServing.Status == statusServing && nowServing.StatusChangeAt == Time)
        {
            nowServing.Status = statusServed;
            outChannel->Push(nowServing);
        }
        if (!inChannel->IsEmpty())
        {
            if (nowServing.Status == statusServing)
            {
                orbitAppendChannel->Push(inChannel->Pop());
            }
            else
            {
                nowServing = inChannel->Pop();
                nowServing.StatusChangeAt = inputDelay->Get();
                nowServing.Status = statusServing;
                EventQueue.push_back(nowServing.StatusChangeAt);
            }
        }

        if (!orbitChannel->IsEmpty())
        {
            if (nowServing.Status == statusServing)
            {
                orbitAppendChannel->Push(orbitChannel->Pop());
            }
            else
            {
                nowServing = orbitChannel->Pop();
                nowServing.StatusChangeAt = inputDelay->Get();
                nowServing.Status = statusServing;
                EventQueue.push_back(nowServing.StatusChangeAt);
            }
        }

        if (!callChannel->IsEmpty())
        {
            if (nowServing.Status != statusServing)
            {
                nowServing = callChannel->Pop();
                nowServing.StatusChangeAt = calledDelay->Get();
                nowServing.Status = statusServing;
                EventQueue.push_back(nowServing.StatusChangeAt);
            }
            else
            {
                callChannel->Pop();
            }
        }
    }
};

class SimpleNode : public Producer
{
    Request nowServing;
    Delay *delay;
    Router *inChannel;
    Router *outChannel;

public:
    SimpleNode(Delay *delay, Router *inChannel, Router *outChannel)
    {
        this->delay = delay;
        this->inChannel = inChannel;
        this->outChannel = outChannel;
        this->nowServing = Request{Status : statusServed};
    }

    void Produce()
    {
        if (nowServing.Status == statusServing && nowServing.StatusChangeAt == Time)
        {
            nowServing.Status = statusServed;
            outChannel->Push(nowServing);
        }
        if (!inChannel->IsEmpty())
        {
            if (nowServing.Status != statusServing)
            {
                nowServing = inChannel->Pop();
                nowServing.StatusChangeAt = delay->Get();
                nowServing.Status = statusServing;
                EventQueue.push_back(nowServing.StatusChangeAt);
            }
        }
    }
};

class OrbitNode : Producer
{
    Request nowServing;
    Delay *delay;
    Router *inChannel;
    Router *orbitChannel;
    Router *orbitAppendChannel;
    Router *outChannel;

public:
    OrbitNode(Delay *delay,
              Router *inChannel,
              Router *orbitChannel,
              Router *orbitAppendChannel,
              Router *outChannel)
    {
        this->delay = delay;
        this->inChannel = inChannel;
        this->orbitChannel = orbitChannel;
        this->orbitAppendChannel = orbitAppendChannel;
        this->outChannel = outChannel;
        this->nowServing = Request{Status : statusServed};
    }

    void Produce()
    {
        if (nowServing.Status == statusServing && nowServing.StatusChangeAt == Time)
        {
            nowServing.Status = statusServed;
            outChannel->Push(nowServing);
        }
        if (!inChannel->IsEmpty())
        {
            if (nowServing.Status == statusServing)
            {
                orbitAppendChannel->Push(inChannel->Pop());
            }
            else
            {
                nowServing = inChannel->Pop();
                nowServing.StatusChangeAt = delay->Get();
                nowServing.Status = statusServing;
                EventQueue.push_back(nowServing.StatusChangeAt);
            }
        }

        if (!orbitChannel->IsEmpty())
        {
            if (nowServing.Status == statusServing)
            {
                orbitAppendChannel->Push(orbitChannel->Pop());
            }
            else
            {
                nowServing = orbitChannel->Pop();
                nowServing.StatusChangeAt = delay->Get();
                nowServing.Status = statusServing;
                EventQueue.push_back(nowServing.StatusChangeAt);
            }
        }
    }
};

#endif