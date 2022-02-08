#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <queue>
#include "request.hpp"
class Router
{
    std::vector<Request> q;

public:
    virtual Request Pop()
    {
        Request ret = q.front();
        q.erase(q.begin());
        return ret;
    }

    virtual int Len()
    {
        return q.size();
    }

    virtual void Push(Request request)
    {
        q.push_back(request);
    }

    virtual bool IsEmpty()
    {
        return q.empty();
    }
};

class NoneRouter : public Router
{
public:
    Request Pop() override
    {
        return Request{};
    }
    int Len()
    {
        return 0;
    }

    void Push(Request request)
    {
    }

    bool IsEmpty()
    {
        return true;
    }
};

#endif