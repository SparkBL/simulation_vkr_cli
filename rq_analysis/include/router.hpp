#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <queue>
#include <unordered_map>
#include "reader.hpp"

class Router
{
public:
    std::vector<Request> q;
    std::unordered_map<std::string, RouterReader *> readers;
    friend class InSlot;
    friend class OutSlot;
    friend class Slot;
    int pushed_count;
    int popped_count;

    Router()
    {
        readers = {};
        q = {};
        pushed_count = 0;
        popped_count = 0;
    }

    void AddReader(RouterReader *r, std::string label)
    {
        if (readers.count(label))
        {
            throw std::invalid_argument(label + " already exists in readers");
        }
        if (r == nullptr)
        {
            throw std::invalid_argument("router reader objet is nil");
        }

        readers.insert(readers.end(), std::pair<std::string, RouterReader *>(label, r));
    }

    const std::unordered_map<std::string, RouterReader *> *Readers()
    {
        const std::unordered_map<std::string, RouterReader *> *p = &readers;
        return p;
    }

    RouterReader *ReaderAt(std::string label)
    {
        if (!readers.count(label))
        {
            throw std::invalid_argument(label + " does not exists in readers");
        }
        return readers.at(label);
    }

    virtual Request Pop()
    {
        Request ret = q.front();
        q.erase(q.begin());
        popped_count++;
        for (auto &e : readers)
        {
            e.second->Read(&ret);
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

    void Push(Request request)
    {
    }

    bool IsEmpty()
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
    int Len()
    {
        return 0;
    }

    void Push(Request request)
    {
        popped_count++;
        for (auto &e : this->readers)
        {
            e.second->Read(&request);
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
    Slot(Router *r)
    {
        r = r;
    }
    Slot() {}

    void Connect(Router *in)
    {
        r = in;
    }
};

class InSlot : public Slot
{

public:
    InSlot(Router *in) : Slot(in) {}
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
    OutSlot(Router *in) : Slot(in) {}
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