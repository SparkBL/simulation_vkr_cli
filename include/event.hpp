#ifndef EVENT_HPP
#define EVENT_HPP
struct Event
{
    virtual ~Event() {}
};
struct Crash : public Event
{
    // object *collider;
};

struct Bridge
{
    virtual ~Bridge() {}
    virtual bool same_as(const Bridge *p) const = 0; //to implement unlisten
    virtual bool on_ev(Event &ev) = 0;
};

template <class E, class T>
struct fnbridge : public Bridge
{
    T *pt;
    bool (T::*mfn)(E &);

    virtual bool on_ev(Event &ev)
    {
        E *pe = dynamic_cast<E *>(&ev);
        return pe && (pt->*mfn)(*pe);
    }

    virtual bool same_as(const Bridge *p)
    {
        const fnbridge *pb = dynamic_cast<const fnbridge *>(p);
        return pb && pb->pt == pt && pb->mfn == mfn;
    }
};

#endif