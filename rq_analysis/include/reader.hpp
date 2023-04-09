#ifndef READER_HPP
#define READER_HPP

#include "request.hpp"
#include <vector>
#include <unordered_map>
// #include "reader.hpp"

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

    virtual void Read(const Request *r) override
    {
        if (r == nullptr)
            return;
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

    std::vector<std::vector<double>> GetDistribution()
    {
        double dsize_x = 0, dsize_y = 0;
        for (int i = 0; i < interval_stats.size(); i++)
        {
            if (interval_stats[i].input > dsize_x)
            {
                dsize_x = interval_stats[i].input;
            }
            if (interval_stats[i].called > dsize_y)
            {
                dsize_y = interval_stats[i].called;
            }
        }
        std::vector<double> y(dsize_y + 1, 0.0);
        std::vector<std::vector<double>> distr(dsize_x + 1, y);

        for (int i = 0; i < interval_stats.size(); i++)
        {
            distr[interval_stats[i].input][interval_stats[i].called] += 1.0;
        }
        double norm = double(interval_stats.size());
        for (int i = 0; i < distr.size(); i++)
        {
            for (int j = 0; j < distr[i].size(); j++)
            {
                distr[i][j] /= norm;
            }
        }
        return distr;
    }

    std::vector<double> GetSummaryDistribution()
    {
        double dsize = 0;
        for (int i = 0; i < interval_stats.size(); i++)
        {
            if (interval_stats[i].input + interval_stats[i].called > dsize)
            {
                dsize = interval_stats[i].input + interval_stats[i].called;
            }
        }
        std::vector<double> distr(dsize + 1);

        for (int i = 0; i < interval_stats.size(); i++)
        {
            distr[interval_stats[i].input + interval_stats[i].called] += 1.0;
        }
        double norm = double(interval_stats.size());
        for (int i = 0; i < distr.size(); i++)
        {
            distr[i] /= norm;
        }
        return distr;
    }

    std::vector<double> GetInputDistribution()
    {
        double dsize = 0;
        for (int i = 0; i < interval_stats.size(); i++)
        {
            if (interval_stats[i].input > dsize)
            {
                dsize = interval_stats[i].input;
            }
        }
        std::vector<double> distr(dsize + 1);

        for (int i = 0; i < interval_stats.size(); i++)
        {
            distr[interval_stats[i].input] += 1.0;
        }
        double norm = double(interval_stats.size());
        for (int i = 0; i < distr.size(); i++)
        {
            distr[i] /= norm;
        }
        return distr;
    }

    std::vector<double> GetCalledDistribution()
    {
        double dsize = 0;
        for (int i = 0; i < interval_stats.size(); i++)
        {
            if (interval_stats[i].called > dsize)
            {
                dsize = interval_stats[i].called;
            }
        }
        std::vector<double> distr(dsize + 1);

        for (int i = 0; i < interval_stats.size(); i++)
        {
            distr[interval_stats[i].called] += 1.0;
        }
        double norm = double(interval_stats.size());
        for (int i = 0; i < distr.size(); i++)
        {
            distr[i] /= norm;
        }
        return distr;
    }

    double GetMeanInput()
    {
        double sum = 0;
        for (auto it = interval_stats.begin(); it != interval_stats.end(); it++)
            sum += double(it->input);
        return sum / double(interval_stats.size());
    }

    double GetMeanCalled()
    {
        double sum = 0;
        for (auto it = interval_stats.begin(); it != interval_stats.end(); it++)
            sum += double(it->called);
        return sum / double(interval_stats.size());
    }

    double GetMeanIntervalInput()
    {
        double sum = 0;
        for (auto it = time_deltas_input.begin(); it != time_deltas_input.end(); it++)
            sum += *it;
        return sum / double(time_deltas_input.size());
    }

    double GetSqMeanIntervalInput()
    {
        double sum = 0;
        for (auto it = time_deltas_input.begin(); it != time_deltas_input.end(); it++)
            sum += *it * *it;
        return sum / double(time_deltas_input.size());
    }
    double GetDispersionIntervalInput()
    {
        return GetSqMeanIntervalInput() - GetMeanIntervalInput() * GetMeanIntervalInput();
    }

    double GetVariationIntervalInput()
    {
        return std::sqrt(GetDispersionIntervalInput()) / GetMeanIntervalInput();
    }

    double GetMeanIntervalCalled()
    {
        double sum = 0;
        for (auto it = time_deltas_called.begin(); it != time_deltas_called.end(); it++)
            sum += *it;
        return sum / double(time_deltas_called.size());
    }

    double GetSqMeanIntervalCalled()
    {
        double sum = 0;
        for (auto it = time_deltas_called.begin(); it != time_deltas_called.end(); it++)
            sum += *it * *it;
        return sum / double(time_deltas_called.size());
    }
    double GetDispersionIntervalCalled()
    {
        return GetSqMeanIntervalCalled() - GetMeanIntervalCalled() * GetMeanIntervalCalled();
    }

    double GetVariationIntervalCalled()
    {
        return std::sqrt(GetDispersionIntervalCalled()) / GetMeanIntervalCalled();
    }
};

class AttemptCounter : public RouterReader
{
public:
    std::unordered_map<unsigned int, int> attempts;
    std::unordered_map<unsigned int, double> wait_time;

    AttemptCounter()
    {
        attempts = {};
    }

    virtual void Read(const Request *r) override
    {
        if (r == nullptr)
            return;
        this->attempts[r->id]++;
        this->wait_time[r->id] = r->status_change_at - r->emitted_at;
    }
};

class TimeCounter : public RouterReader
{
public:
    std::vector<double> counts;

    TimeCounter()
    {
        counts = {};
    }

    virtual void Read(const Request *r) override
    {
        counts.push_back(r->status_change_at);
    }
};

#endif