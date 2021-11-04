#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <queue>
#include "request.hpp"
class Router
{
    std::queue<Request *> q;

public:
    Request *Pop()
    {
        Request *ret = q.front();
        q.pop();
        return ret;
    }

    int Len()
    {
        return q.size();
    }

    void Push(Request *request)
    {
        q.push(request);
    }
};

#endif