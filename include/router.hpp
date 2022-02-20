#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <queue>
#include "request.hpp"

//namespace connections
//{
class Router
{
    std::vector<Request> q_;
    friend class InSlot;
    friend class OutSlot;

private:
    virtual Request Pop()
    {
        Request ret = q_.front();
        q_.erase(q_.begin());
        return ret;
    }

    virtual int Len()
    {
        return q_.size();
    }

    virtual void Push(Request request)
    {
        q_.push_back(request);
    }

    virtual bool IsEmpty()
    {
        return q_.empty();
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

class Slot
{
protected:
    Router *r_;

public:
    Slot(Router *r)
    {
        r_ = r;
    }
    Slot() {}

    void Connect(Router *in)
    {
        r_ = in;
    }
};

class InSlot : public Slot
{

public:
    InSlot(Router *in) : Slot(in) {}
    InSlot() : Slot() {}

    int Len()
    {
        return r_->Len();
    }
    bool IsEmpty()
    {
        return r_->IsEmpty();
    }

    Request Pop()
    {
        return r_->Pop();
    }
};

class OutSlot : public Slot
{

    Router *r_;

public:
    OutSlot(Router *in) : Slot(in) {}
    OutSlot() : Slot() {}

    int Len()
    {
        return r_->Len();
    }

    void Push(Request request)
    {
        r_->Push(request);
    }
};
//};
#endif