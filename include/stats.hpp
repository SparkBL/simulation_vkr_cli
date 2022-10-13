#ifndef STATS_HPP
#define STATS_HPP

#include <vector>
#include "router.hpp"
#include "request.hpp"
#include "producer.hpp"
#include "math.h"
#include <unordered_map>
//#include "easy/profiler.h"
struct IntervalStat
{
    int input;
    int called;
};

class StatCollector : public Producer
{
    std::vector<IntervalStat> interval_stats_;
    std::vector<double> time_deltas_input_;
    std::vector<double> time_deltas_called_;
    InSlot output_channel_;
    IntervalStat cur_;
    double cur_interval_;
    double interval_;
    double last_delta_input_;
    double last_delta_called_;

public:
    StatCollector(double interval)
    {
        this->interval_ = interval;
        this->cur_interval_ = interval;
        this->cur_ = IntervalStat{input : 0, called : 0};
        this->last_delta_input_ = 0;
        this->last_delta_called_ = 0;
        outputs_ = {};
        inputs_ = {{"in_slot", &output_channel_}};
    }

    std::vector<double> Produce(double time) override
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
            switch (r.rtype)
            {
            case typeInput:
                cur_.input++;
                time_deltas_input_.push_back(r.status_change_at - last_delta_input_);
                last_delta_input_ = r.status_change_at;
                break;
            case typeCalled:
                cur_.called++;
                time_deltas_called_.push_back(r.status_change_at - last_delta_input_);
                last_delta_called_ = r.status_change_at;
                break;
            }
        }
        return GetEvents();
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

    double GetMeanIntervalInput()
    {
        double sum = 0;
        for (auto it = time_deltas_input_.begin(); it != time_deltas_input_.end(); it++)
            sum += *it;
        return sum / double(time_deltas_input_.size());
    }

    double GetSqMeanIntervalInput()
    {
        double sum = 0;
        for (auto it = time_deltas_input_.begin(); it != time_deltas_input_.end(); it++)
            sum += *it * *it;
        return sum / double(time_deltas_input_.size());
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
        for (auto it = time_deltas_called_.begin(); it != time_deltas_called_.end(); it++)
            sum += *it;
        return sum / double(time_deltas_called_.size());
    }

    double GetSqMeanIntervalCalled()
    {
        double sum = 0;
        for (auto it = time_deltas_called_.begin(); it != time_deltas_called_.end(); it++)
            sum += *it * *it;
        return sum / double(time_deltas_called_.size());
    }
    double GetDispersionIntervalCalled()
    {
        return GetSqMeanIntervalCalled() - GetMeanIntervalCalled() * GetMeanIntervalCalled();
    }

    double GetVariationIntervalCalled()
    {
        return std::sqrt(GetDispersionIntervalCalled()) / GetMeanIntervalCalled();
    }

    std::string Tag() override
    {
        return "stat_collector";
    }
};
/*
class TimedStatCollector
{
    std::vector<std::vector<double>> distr;
    Router *outputChannel;
    int maxInput, maxCalled, curInputCount, curCalledCount;
    double prevInput, prevCalled;
    std::vector<double> curInputQueue, curCalledQueue;

public:
    TimedStatCollector(Router *outputChannel)
    {
        this->outputChannel = outputChannel;
        maxInput = 0;
        maxCalled = 0;
        curInputCount = 0;
        curCalledCount = 0;
        std::vector<double> y(150, 0.0);
        distr = std::vector<std::vector<double>>(150, y);
    }
    void GatherStat()
    {
        while (!outputChannel->IsEmpty())
        {
            Request r = outputChannel->Pop();
            switch (r.Type)
            {
            case TypeInput:
                while (!curInputQueue.empty())
                {
                    if (*curInputQueue.begin() < r.StatusChangeAt)
                    {
                        distr[curInputCount][curCalledCount] += curInputQueue[0] - prevInput;
                        curInputCount--;
                        prevInput = curInputQueue[0];
                        curInputQueue.erase(curInputQueue.begin());
                    }
                    else
                        break;
                }
                distr[curInputCount][curCalledCount] += r.StatusChangeAt - prevInput;
                curInputCount++;
                curInputQueue.push_back(r.StatusChangeAt + Interval);
                prevInput = r.StatusChangeAt;
                if (curInputCount > maxInput)
                {
                    maxInput = curInputCount;
                }
                break;
            case TypeCalled:
                while (!curCalledQueue.empty())
                {
                    if (*curCalledQueue.begin() < r.StatusChangeAt)
                    {
                        distr[curInputCount][curCalledCount] += curCalledQueue[0] - prevInput;
                        curCalledCount--;
                        prevCalled = curCalledQueue[0];
                        curCalledQueue.erase(curCalledQueue.begin());
                    }
                    else
                        break;
                }
                distr[curInputCount][curCalledCount] += r.StatusChangeAt - prevCalled;
                curCalledCount++;
                curCalledQueue.push_back(r.StatusChangeAt + Interval);
                prevCalled = r.StatusChangeAt;
                if (curCalledCount > maxCalled)
                {
                    maxCalled = curCalledCount;
                }
                break;
            }
        }
    }

    std::vector<std::vector<double>> GetDistribution()
    {
        maxInput++;
        maxCalled++;
        double norm = 0.0;
        for (int i = 0; i < distr.size(); i++)
            for (int j = 0; j < distr[i].size(); j++)
                norm += distr[i][j];
        std::vector<double> y(maxCalled, 0.0);
        std::vector<std::vector<double>> ret(maxInput, y);

        for (int i = 0; i < maxInput; i++)
            for (int j = 0; j < maxCalled; j++)
                ret[i][j] = distr[i][j] / norm;
        return ret;
    }
};

class OrbitStatCollector
{
    std::vector<double> distr;
    Router *outputChannel;
    int curCount, maxCount;
    double curTime;

public:
    OrbitStatCollector(Router *outputChannel)
    {
        this->outputChannel = outputChannel;
        curCount = maxCount = 0;
        curTime = 0.0;
        distr = std::vector<double>(1000, 0.0);
    }

    void GatherStat()
    {
        while (!outputChannel->IsEmpty())
        {
            Request r = outputChannel->Pop();
            distr[curCount] = r.StatusChangeAt - curTime;
            curTime = r.StatusChangeAt;
            switch (r.Status)
            {
            case statusArrive:
            {
                curCount++;
                if (maxCount < curCount)
                    maxCount = curCount;
                break;
            }
            case statusLeave:
            {
                curCount--;
                break;
            }
            }
        }
    }

    std::vector<double> GetDistribution()
    {
        double norm = 0.0;
        for (int i = 0; i < maxCount + 1; i++)
            norm += distr[i];

        for (int i = 0; i < maxCount + 1; i++)
            distr[i] = distr[i] / norm;
        return std::vector<double>(distr.begin(), distr.begin() + maxCount);
    }
};*/
#endif