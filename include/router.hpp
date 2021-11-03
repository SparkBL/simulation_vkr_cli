#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <vector>
#include "request.hpp"
class Router
{
    std::vector<Request *> q;

public:
    Request *Pop()
    {
        Request *ret = *q.begin();
        q.erase(q.begin());
        return ret;
    }

    int Len()
    {
        return q.size();
    }

    void Push(Request *request)
    {
        q.push_back(request);
    }
};

#endif