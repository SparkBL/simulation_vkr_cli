#ifndef ROUTER_READER_HPP
#define ROUTER_READER_HPP

#include "request.hpp"
#include "stats.hpp"

class RouterReader
{
public:
    virtual void Read(const Request *r) = 0;
    virtual ~RouterReader() = default;
};

class IntervalRouterReader : public RouterReader
{
private:
    std::vector<IntervalStat> interval_stats;
    std::vector<double> time_deltas_input;
    std::vector<double> time_deltas_called;
    std::vector<double> time_deltas_state;
    IntervalStat cur;
    double cur_interval;
    double interval;
    double last_delta_input;
    double last_delta_called;
    double last_delta_state;

public:
    IntervalRouterReader(double interval)
    {
        this->interval = interval;
        this->cur_interval = interval;
        this->cur = IntervalStat{input : 0, called : 0};
        this->last_delta_input = 0;
        this->last_delta_called = 0;
    }

    void Read(const Request *r) override
    {
        while (r->status_change_at > cur_interval)
        {
            interval_stats.push_back(cur);
            cur = IntervalStat{input : 0, called : 0};
            cur_interval += interval;
        }
        switch (r->rtype)
        {
        case typeInput:
            cur.input++;
            time_deltas_input.push_back(r->status_change_at - last_delta_input);
            last_delta_input = r->status_change_at;
            break;
        case typeCalled:
            cur.called++;
            time_deltas_called.push_back(r->status_change_at - last_delta_input);
            last_delta_called = r->status_change_at;
            break;
        case typeState:
            cur.state++;
            time_deltas_state.push_back(r->status_change_at - last_delta_input);
            last_delta_state = r->status_change_at;
            break;
        }
    }
};
#endif