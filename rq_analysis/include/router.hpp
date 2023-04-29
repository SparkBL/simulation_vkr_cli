#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <queue>
#include <unordered_map>
#include "reader.hpp"
#include <iostream>
class Router
{
public:
    std::vector<Request> q;
    std::unordered_map<std::string, RouterReader &> readers;
    friend class InSlot;
    friend class OutSlot;
    friend class Slot;
    int pushed_count;
    int popped_count;

    virtual ~Router() = default;

    Router()
    {
        readers = {};
        q = {};
        pushed_count = 0;
        popped_count = 0;
    }

    void AddReader(RouterReader &r, std::string label)
    {
        if (readers.count(label))
        {
            throw std::invalid_argument(label + " already exists in readers");
        }
        readers.insert(readers.end(), std::pair<std::string, RouterReader &>(label, r));
    }

    std::unordered_map<std::string, RouterReader &> &Readers()
    {
        return readers;
    }

    RouterReader &ReaderAt(std::string label)
    {
        if (!readers.count(label))
        {
            throw std::invalid_argument(label + " does not exists in readers");
        }
        return readers.at(label);
        ;
    }

    virtual Request Pop()
    {
        if (q.size() == 0)
        {
            throw std::invalid_argument("router is empty");
        }
        Request ret = q.front();
        q.erase(q.begin());
        popped_count++;
        for (auto &e : readers)
        {
            e.second.Read(ret);
        }
        return ret;
    }

    virtual int Len()
    {
        return q.size();
    }

    virtual void Push(Request request)
    {
        q.push_back(request);
        pushed_count++;
    }

    virtual bool IsEmpty()
    {
        return q.empty();
    }
};

class NoneRouter : public Router
{
public:
    NoneRouter() : Router() {}

    Request Pop() override
    {
        return Request{};
    }
    int Len()
    {
        return 0;
    }

    void Push(Request request) override
    {
        std::cout << "pushing into NoneRouter" << std::endl;
    }

    bool IsEmpty() override
    {
        return true;
    }
};

class OutputRouter : public Router
{
public:
    OutputRouter() : Router() {}

    Request Pop() override
    {
        return Request{};
    }
    int Len() override
    {
        return 0;
    }

    void Push(Request request) override
    {
        popped_count++;
        for (auto &e : this->readers)
        {
            e.second.Read(request);
        }
    }

    bool IsEmpty()
    {
        return true;
    }
};

class Slot
{
protected:
    Router *r;

public:
    Slot(Router &r)
    {
        this->r = &r;
    }
    Slot()
    {
        r = new NoneRouter();
    }
    void Connect(Router &in)
    {
        r = &in;
        // std::cout << "connected" << std::endl;
    }
};

class InSlot : public Slot
{

public:
    InSlot(Router &in) : Slot(in) {}
    InSlot() : Slot() {}
    int Len()
    {
        return Slot::r->Len();
    }
    bool IsEmpty()
    {
        return Slot::r->IsEmpty();
    }

    Request Pop()
    {
        return Slot::r->Pop();
    }
};

class OutSlot : public Slot
{

public:
    OutSlot(Router &in) : Slot(in) {}
    OutSlot() : Slot() {}

    int Len()
    {
        return Slot::r->Len();
    }

    void Push(Request request)
    {
        Slot::r->Push(request);
    }
};

//};
#endif