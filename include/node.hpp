#ifndef NODE_HPP
#define NODE_HPP

#include "delay.hpp"
#include "env.hpp"
#include "router.hpp"
#include "request.hpp"
//#include "easy/profiler.h"
class Node
{
    Request *nowServing;
    Delay *inputDelay;
    Delay *calledDelay;
    Router *inChannel;
    Router *callChannel;
    Router *orbitChannel;
    Router *orbitAppendChannel;
    Router *outChannel;

public:
    Node(Delay *inputDelay,
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
        this->nowServing = new Request{Status : statusServed};
    }

    void Produce()
    {
        //  EASY_FUNCTION(profiler::colors::Red100);
        if (nowServing->Status == statusServing && nowServing->StatusChangeAt == Time)
        {
            //   EASY_BLOCK("filling output", profiler::colors::Red);
            nowServing->Status = statusServed;
            outChannel->Push(nowServing);
        }
        if (inChannel->Len() > 0)
        {
            //  EASY_BLOCK("Processing input", profiler::colors::Red200);
            if (nowServing->Status == statusServing)
            {
                orbitAppendChannel->Push(inChannel->Pop());
            }
            else
            {
                nowServing = inChannel->Pop();
                nowServing->StatusChangeAt = inputDelay->Get();
                nowServing->Status = statusServing;
                EventQueue.push_back(nowServing->StatusChangeAt);
            }
        }

        if (orbitChannel->Len() > 0)
        {
            // EASY_BLOCK("Processing orbit", profiler::colors::Red300);
            if (nowServing->Status == statusServing)
            {
                orbitAppendChannel->Push(orbitChannel->Pop());
            }
            else
            {
                nowServing = orbitChannel->Pop();
                nowServing->StatusChangeAt = inputDelay->Get();
                nowServing->Status = statusServing;
                EventQueue.push_back(nowServing->StatusChangeAt);
            }
        }

        if (callChannel->Len() > 0)
        {
            //  EASY_BLOCK("Processing called", profiler::colors::Red400);
            if (nowServing->Status != statusServing)
            {
                nowServing = callChannel->Pop();
                nowServing->StatusChangeAt = calledDelay->Get();
                nowServing->Status = statusServing;
                EventQueue.push_back(nowServing->StatusChangeAt);
            }
            else
            {
                delete callChannel->Pop();
            }
        }
    }
};

#endif