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
        //   EASY_FUNCTION(profiler::colors::BlueA100);
        for (int i = 0; i < outputChannel->Len(); i++)
        {
            Request *r = outputChannel->Pop();
            while (r->StatusChangeAt > curInterval)
            {
                intervalStats.push_back(cur);
                cur = IntervalStat{input : 0, called : 0};
                curInterval += Interval;
            }
            switch (r->Type)
            {
            case TypeInput:
                cur.input++;
                break;
            case TypeCalled:
                cur.called++;
                break;
            }
            delete r;
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
            distr[intervalStats[i].input][intervalStats[i].called]++;
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
};

#endif