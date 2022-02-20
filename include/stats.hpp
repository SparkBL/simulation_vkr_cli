#ifndef STATS_HPP
#define STATS_HPP

#include <vector>
#include "env.hpp"
#include "router.hpp"
#include "request.hpp"

//namespace collectors
//{
struct IntervalStat
{
    int input;
    int called;
};

class StatCollector : public Producer
{
public:
    virtual ~StatCollector() {}
    virtual void GatherStat() = 0;
};

class TOutputStatCollector : public StatCollector
{
    std::vector<IntervalStat> interval_stats_;
    InSlot output_channel_;
    IntervalStat cur_;
    double cur_interval_;
    double interval_;

public:
    TOutputStatCollector(double interval)
    {
        this->interval_ = interval;
        this->cur_interval_ = interval;
        this->cur_ = IntervalStat{input : 0, called : 0};
    }

    void Produce()
    {
        while (!output_channel_.IsEmpty())
        {
            Request r = output_channel_.Pop();
            while (r.status_change_at > cur_interval_)
            {
                interval_stats_.push_back(cur_);
                cur_ = IntervalStat{input : 0, called : 0};
                cur_interval_ += interval_;
            }
            switch (r.type)
            {
            case typeInput:
                cur_.input++;
                break;
            case typeCalled:
                cur_.called++;
                break;
            }
        }
    }

    std::vector<std::vector<double>> GetDistribution()
    {
        double dsize_x = 0, dsize_y = 0;
        for (int i = 0; i < interval_stats_.size(); i++)
        {
            if (interval_stats_[i].input > dsize_x)
            {
                dsize_x = interval_stats_[i].input;
            }
            if (interval_stats_[i].called > dsize_y)
            {
                dsize_y = interval_stats_[i].called;
            }
        }
        std::vector<double> y(dsize_y + 1, 0.0);
        std::vector<std::vector<double>> distr(dsize_x + 1, y);

        for (int i = 0; i < interval_stats_.size(); i++)
        {
            distr[interval_stats_[i].input][interval_stats_[i].called] += 1.0;
        }
        double norm = double(interval_stats_.size());
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
        for (int i = 0; i < interval_stats_.size(); i++)
        {
            if (interval_stats_[i].input + interval_stats_[i].called > dsize)
            {
                dsize = interval_stats_[i].input + interval_stats_[i].called;
            }
        }
        std::vector<double> distr(dsize + 1);

        for (int i = 0; i < interval_stats_.size(); i++)
        {
            distr[interval_stats_[i].input + interval_stats_[i].called] += 1.0;
        }
        double norm = double(interval_stats_.size());
        for (int i = 0; i < distr.size(); i++)
        {
            distr[i] /= norm;
        }
        return distr;
    }

    std::vector<double> GetInputDistribution()
    {
        double dsize = 0;
        for (int i = 0; i < interval_stats_.size(); i++)
        {
            if (interval_stats_[i].input > dsize)
            {
                dsize = interval_stats_[i].input;
            }
        }
        std::vector<double> distr(dsize + 1);

        for (int i = 0; i < interval_stats_.size(); i++)
        {
            distr[interval_stats_[i].input] += 1.0;
        }
        double norm = double(interval_stats_.size());
        for (int i = 0; i < distr.size(); i++)
        {
            distr[i] /= norm;
        }
        return distr;
    }

    std::vector<double> GetCalledDistribution()
    {
        double dsize = 0;
        for (int i = 0; i < interval_stats_.size(); i++)
        {
            if (interval_stats_[i].called > dsize)
            {
                dsize = interval_stats_[i].called;
            }
        }
        std::vector<double> distr(dsize + 1);

        for (int i = 0; i < interval_stats_.size(); i++)
        {
            distr[interval_stats_[i].called] += 1.0;
        }
        double norm = double(interval_stats_.size());
        for (int i = 0; i < distr.size(); i++)
        {
            distr[i] /= norm;
        }
        return distr;
    }

    double GetMeanInput()
    {
        double sum = 0;
        for (auto it = interval_stats_.begin(); it != interval_stats_.end(); it++)
            sum += double(it->input);
        return sum / double(interval_stats_.size());
    }

    double GetMeanCalled()
    {
        double sum = 0;
        for (auto it = interval_stats_.begin(); it != interval_stats_.end(); it++)
            sum += double(it->called);
        return sum / double(interval_stats_.size());
    }

    Slot *operator[](std::string slot_name) override
    {
        if (slot_name == "in_slot")
            return &output_channel_;
        return nullptr;
    }

    std::vector<std::string> GetSlotNames() override
    {
        return std::vector<std::string>{"in_slot"};
    }
};

class TimedStatCollector : public StatCollector
{
    std::vector<std::vector<double>> distr_;
    InSlot output_channel_;
    int max_input_, max_called_, cur_input_count_, cur_called_count_;
    double prev_input_, prev_called_;
    double interval_;
    std::vector<double> cur_InputQueue, cur_CalledQueue;

public:
    TimedStatCollector(Router *output_channel, double interval)
    {
        this->output_channel_.Connect(output_channel);
        this->interval_ = interval;
        max_input_ = 0;
        max_called_ = 0;
        cur_input_count_ = 0;
        cur_called_count_ = 0;
        std::vector<double> y(150, 0.0);
        distr_ = std::vector<std::vector<double>>(150, y);
    }
    void GatherStat()
    {
        while (!output_channel_.IsEmpty())
        {
            Request r = output_channel_.Pop();
            switch (r.type)
            {
            case typeInput:
                while (!cur_InputQueue.empty())
                {
                    if (*cur_InputQueue.begin() < r.status_change_at)
                    {
                        distr_[cur_input_count_][cur_called_count_] += cur_InputQueue[0] - prev_input_;
                        cur_input_count_--;
                        prev_input_ = cur_InputQueue[0];
                        cur_InputQueue.erase(cur_InputQueue.begin());
                    }
                    else
                        break;
                }
                distr_[cur_input_count_][cur_called_count_] += r.status_change_at - prev_input_;
                cur_input_count_++;
                cur_InputQueue.push_back(r.status_change_at + interval_);
                prev_input_ = r.status_change_at;
                if (cur_input_count_ > max_input_)
                {
                    max_input_ = cur_input_count_;
                }
                break;
            case typeCalled:
                while (!cur_CalledQueue.empty())
                {
                    if (*cur_CalledQueue.begin() < r.status_change_at)
                    {
                        distr_[cur_input_count_][cur_called_count_] += cur_CalledQueue[0] - prev_input_;
                        cur_called_count_--;
                        prev_called_ = cur_CalledQueue[0];
                        cur_CalledQueue.erase(cur_CalledQueue.begin());
                    }
                    else
                        break;
                }
                distr_[cur_input_count_][cur_called_count_] += r.status_change_at - prev_called_;
                cur_called_count_++;
                cur_CalledQueue.push_back(r.status_change_at + interval_);
                prev_called_ = r.status_change_at;
                if (cur_called_count_ > max_called_)
                {
                    max_called_ = cur_called_count_;
                }
                break;
            }
        }
    }

    std::vector<std::vector<double>> GetDistribution()
    {
        max_input_++;
        max_called_++;
        double norm = 0.0;
        for (int i = 0; i < distr_.size(); i++)
            for (int j = 0; j < distr_[i].size(); j++)
                norm += distr_[i][j];
        std::vector<double> y(max_called_, 0.0);
        std::vector<std::vector<double>> ret(max_input_, y);

        for (int i = 0; i < max_input_; i++)
            for (int j = 0; j < max_called_; j++)
                ret[i][j] = distr_[i][j] / norm;
        return ret;
    }
};

class OrbitStatCollector : public StatCollector
{
    std::vector<double> distr_;
    InSlot output_channel_;
    int cur_count_, max_count_;
    double cur_time_;

public:
    OrbitStatCollector(Router *output_channel)
    {
        this->output_channel_.Connect(output_channel);
        cur_count_ = max_count_ = 0;
        cur_time_ = 0.0;
        distr_ = std::vector<double>(1000, 0.0);
    }

    void GatherStat()
    {
        while (!output_channel_.IsEmpty())
        {
            Request r = output_channel_.Pop();
            distr_[cur_count_] = r.status_change_at - cur_time_;
            cur_time_ = r.status_change_at;
            switch (r.status)
            {
            case statusArrive:
            {
                cur_count_++;
                if (max_count_ < cur_count_)
                    max_count_ = cur_count_;
                break;
            }
            case statusLeave:
            {
                cur_count_--;
                break;
            }
            }
        }
    }

    std::vector<double> GetDistribution()
    {
        double norm = 0.0;
        for (int i = 0; i < max_count_ + 1; i++)
            norm += distr_[i];

        for (int i = 0; i < max_count_ + 1; i++)
            distr_[i] = distr_[i] / norm;
        return std::vector<double>(distr_.begin(), distr_.begin() + max_count_);
    }
};
//};
#endif