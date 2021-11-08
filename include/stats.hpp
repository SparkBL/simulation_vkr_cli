#ifndef STATS_HPP
#define STATS_HPP

#include <vector>
#include "env.hpp"
#include "router.hpp"
#include "request.hpp"
//#include "easy/profiler.h"
struct IntervalStat
{
    int input;
    int called;
};

class StatCollector
{
    std::vector<IntervalStat> intervalStats;
    Router *outputChannel;
    IntervalStat cur;
    double curInterval;

public:
    StatCollector(Router *outputChannel)
    {
        this->outputChannel = outputChannel;
        this->curInterval = Interval;
        this->cur = IntervalStat{input : 0, called : 0};
    }

    void GatherStat()
    {
        while (!outputChannel->IsEmpty())
        {
            Request r = outputChannel->Pop();
            while (r.StatusChangeAt > curInterval)
            {
                intervalStats.push_back(cur);
                cur = IntervalStat{input : 0, called : 0};
                curInterval += Interval;
            }
            switch (r.Type)
            {
            case TypeInput:
                cur.input++;
                break;
            case TypeCalled:
                cur.called++;
                break;
            }
        }
    }

    std::vector<std::vector<double>> GetDistribution()
    {
        double distSizeX = 0, distSizeY = 0;
        for (int i = 0; i < intervalStats.size(); i++)
        {
            if (intervalStats[i].input > distSizeX)
            {
                distSizeX = intervalStats[i].input;
            }
            if (intervalStats[i].called > distSizeY)
            {
                distSizeY = intervalStats[i].called;
            }
        }
        std::vector<double> y(distSizeY + 1, 0.0);
        std::vector<std::vector<double>> distr(distSizeX + 1, y);

        for (int i = 0; i < intervalStats.size(); i++)
        {
            distr[intervalStats[i].input][intervalStats[i].called] += 1.0;
        }
        double norm = double(intervalStats.size());
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
        double distSize = 0;
        for (int i = 0; i < intervalStats.size(); i++)
        {
            if (intervalStats[i].input + intervalStats[i].called > distSize)
            {
                distSize = intervalStats[i].input + intervalStats[i].called;
            }
        }
        std::vector<double> distr(distSize + 1);

        for (int i = 0; i < intervalStats.size(); i++)
        {
            distr[intervalStats[i].input + intervalStats[i].called] += 1.0;
        }
        double norm = double(intervalStats.size());
        for (int i = 0; i < distr.size(); i++)
        {
            distr[i] /= norm;
        }
        return distr;
    }

    std::vector<double> GetInputDistribution()
    {
        double distSize = 0;
        for (int i = 0; i < intervalStats.size(); i++)
        {
            if (intervalStats[i].input > distSize)
            {
                distSize = intervalStats[i].input;
            }
        }
        std::vector<double> distr(distSize + 1);

        for (int i = 0; i < intervalStats.size(); i++)
        {
            distr[intervalStats[i].input] += 1.0;
        }
        double norm = double(intervalStats.size());
        for (int i = 0; i < distr.size(); i++)
        {
            distr[i] /= norm;
        }
        return distr;
    }

    std::vector<double> GetCalledDistribution()
    {
        double distSize = 0;
        for (int i = 0; i < intervalStats.size(); i++)
        {
            if (intervalStats[i].called > distSize)
            {
                distSize = intervalStats[i].called;
            }
        }
        std::vector<double> distr(distSize + 1);

        for (int i = 0; i < intervalStats.size(); i++)
        {
            distr[intervalStats[i].called] += 1.0;
        }
        double norm = double(intervalStats.size());
        for (int i = 0; i < distr.size(); i++)
        {
            distr[i] /= norm;
        }
        return distr;
    }

    double GetMeanInput()
    {
        double sum = 0;
        for (auto it = intervalStats.begin(); it != intervalStats.end(); it++)
            sum += double(it->input);
        return sum / double(intervalStats.size());
    }

    double GetMeanCalled()
    {
        double sum = 0;
        for (auto it = intervalStats.begin(); it != intervalStats.end(); it++)
            sum += double(it->called);
        return sum / double(intervalStats.size());
    }
};

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
        double norm = 0.0;
        for (int i = 0; i < distr.size(); i++)
            for (int j = 0; j < distr[i].size(); j++)
                norm += distr[i][j];
        std::vector<double> y(maxInput, 0.0);
        std::vector<std::vector<double>> ret(maxCalled, y);

        for (int i = 0; i < distr.size(); i++)
            for (int j = 0; j < distr[i].size(); j++)
                ret[i][j] = distr[i][j] / norm;
        return ret;
    }
};
#endif