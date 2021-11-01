#ifndef STATISTIC_H
#define STATISTIC_H
#include <string>
#include <vector>
#include <algorithm>

struct Statistic
{
    std::string currentEventType;
    int incomeInputProcess =0 ;
    int incomeCalledProcess =0 ;
    int servedInputProcess =0 ;
    int servedCalledProcess = 0;
    mutable std::pair<double,double> currentRunServed = std::pair<double,double>(0,0);
    int orbitSize  = 0;
    std::vector<std::pair<int,int>> *runStats = new std::vector<std::pair<int,int>>();

    void Stat() const;
    std::vector<std::vector<double>> GetDistribution() const;
    std::vector<double> GetSummaryDistribution() const;
    int GetDistributionRowSize() const;
    int GetSummaryDistributionSize() const;
    double GetMeanServedInputProcess() const;
    double GetMeanServedCalledProcess() const;
    ~Statistic();
};

#endif // STATISTIC_H
