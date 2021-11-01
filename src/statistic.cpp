#include "statistic.h"

double Statistic::GetMeanServedInputProcess() const{
    double sum =0 ;
    for (auto it = runStats->begin();it!=runStats->end();it++)
        sum+=double(it->first);
    return sum/double(runStats->size());
}
double Statistic::GetMeanServedCalledProcess() const{
    double sum =0 ;
    for (auto it = runStats->begin();it!=runStats->end();it++)
        sum+=double(it->second);
    return sum/double(runStats->size());
}

void Statistic::Stat() const{
    runStats->push_back(currentRunServed);
    currentRunServed = std::pair<double,double>(0,0);
}


std::vector<std::vector<double>> Statistic::GetDistribution() const{

    int size = runStats->size();
    int resultSize =GetDistributionRowSize();

    std::vector<std::vector<double>> count(resultSize);

    for (int i=0; i<resultSize; i++){
        count[i].resize(resultSize);
        count[i].assign(resultSize,0.0);

    }

    double norm = 0;
    for (int i=0; i<size; i++){
        count[runStats->at(i).first][runStats->at(i).second] +=1.0;
        if (count[runStats->at(i).first][runStats->at(i).second]>0)norm++;
    }

    for (int i=0; i<resultSize; i++)
        for (int j=0; j<resultSize; j++)
            count[i][j]/=norm;

    return  count;
}

std::vector<double> Statistic::GetSummaryDistribution() const{
    int size = runStats->size();
    int resultSize =GetSummaryDistributionSize();

    std::vector<double> count(resultSize);
    count.assign(resultSize,0.0);

    double norm = 0;
    for (int i=0; i<size; i++){
        count[runStats->at(i).first + runStats->at(i).second] +=1.0;
        if (count[runStats->at(i).first + runStats->at(i).second]>0)norm++;
    }

    for (int i=0; i<resultSize; i++)
        count[i]/=norm;
    return  count;
}

int Statistic::GetDistributionRowSize() const{
    int resultSize;
    auto res = std::max_element(runStats->begin(), runStats->end());
    if ( res->first > res->second)resultSize=res->first; else resultSize = res->second;
    return resultSize+3;
}

int Statistic::GetSummaryDistributionSize() const{
    std::vector<double> s(runStats->size());
    s.assign(runStats->size(),0.0);
    for(size_t i = 0;i<s.size();i++)
        s[i] = runStats->at(i).first + runStats->at(i).second;
    return *std::max_element(s.begin(), s.end()) + 2;
}



Statistic::~Statistic(){
    runStats->clear();
    runStats->shrink_to_fit();
    delete runStats;
}
