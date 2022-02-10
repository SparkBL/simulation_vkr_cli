#ifndef DELAY_HPP
#define DELAY_HPP
#include "env.hpp"
#include "math.h"
#include <ctime>
#include <random>
#include <vector>
#include <time.h>
//namespace delays
//{
const double float64_equality_threshold = 1e-11;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<double> distribution(0, 1);
class Delay
{
public:
    virtual double Get() = 0;
};

class ExponentialDelay : public Delay
{
    double intensity_;
    std::exponential_distribution<double> aws_;

public:
    ExponentialDelay(double intensity)
    {
        this->intensity_ = intensity;
        aws_ = std::exponential_distribution<double>(intensity_);
    }
    double Get() override
    {
        return aws_(gen) + Time;
    }
};

class UniformDelay : public Delay
{
    double a_, b_;
    std::uniform_real_distribution<double> aws_;

public:
    UniformDelay(double a, double b)
    {
        this->a_ = a;
        this->b_ = b;
        aws_ = std::uniform_real_distribution<double>(a_, b_);
    }
    double Get() override
    {
        return aws_(gen) + Time;
    }
};

class GammaDelay : public Delay
{
    double k_, teta_;
    std::gamma_distribution<double> aws_;

public:
    GammaDelay(double k, double teta)
    {
        this->k_ = k;
        this->teta_ = teta;
        aws_ = std::gamma_distribution<double>(k_, teta_);
    }
    double Get() override
    {
        return aws_(gen) + Time;
    }
};

inline double GetExponentialDelay(double intensity)
{
    auto aws = std::exponential_distribution<double>(intensity);
    return aws(gen) + Time;
}

inline double NextDouble()
{
    return distribution(gen);
}
//};
#endif