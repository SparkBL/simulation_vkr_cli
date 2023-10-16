#ifndef DELAY_HPP
#define DELAY_HPP
#include "math.h"
#include <ctime>
#include <random>
#include <vector>
#include <time.h>
#include "producer.hpp"

// namespace delays
//{
const double float64_equality_threshold = 1e-11;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<double> distribution(0, 1);
class Delay : public Descriptable
{
public:
    virtual double Get(double time) = 0;
    virtual ~Delay() = default;
};

class ExponentialDelay : public Delay
{
    double intensity;
    std::exponential_distribution<double> aws;

public:
    ExponentialDelay(double intensity)
    {
        this->intensity = intensity;
        aws = std::exponential_distribution<double>(intensity);
    }
    double Get(double time) override
    {
        return aws(gen) + time;
    }
};

class WeibullDelay : public Delay
{
    double a;
    double b;
    std::weibull_distribution<double> aws;

public:
    WeibullDelay(double a, double b)
    {
        this->a = a;
        this->b = b;
        aws = std::weibull_distribution<double>(a, b);
    }
    double Get(double time) override
    {
        return aws(gen) + time;
    }
};

class LognormalDelay : public Delay
{
    double m;
    double s;
    std::lognormal_distribution<double> aws;

public:
    LognormalDelay(double m, double s)
    {
        this->m = m;
        this->s = s;
        aws = std::lognormal_distribution<double>(m, s);
    }
    double Get(double time) override
    {
        return aws(gen) + time;
    }
};

class UniformDelay : public Delay
{
    double a, b;
    std::uniform_real_distribution<double> aws;

public:
    UniformDelay(double a, double b)
    {
        this->a = a;
        this->b = b;
        aws = std::uniform_real_distribution<double>(a, b);
    }
    double Get(double time) override
    {
        return aws(gen) + time;
    }
};

class GammaDelay : public Delay
{
    double k_, teta_;
    std::gamma_distribution<double> aws;

public:
    GammaDelay(double k, double teta)
    {
        this->k_ = k;
        this->teta_ = teta;
        aws = std::gamma_distribution<double>(k_, teta_);
    }
    double Get(double time) override
    {
        return aws(gen) + time;
    }
};

class HyperExponential : public Delay
{
    std::vector<std::exponential_distribution<double>> aws;
    std::vector<double> p;
    std::discrete_distribution<int> dist;

public:
    HyperExponential(const std::vector<double> &p, const std::vector<double> &i)
    {
        this->p = p;
        for (auto e : i)
        {
            aws.push_back(std::exponential_distribution<double>(e));
        }

        this->dist = std::discrete_distribution<int>(p.begin(), p.end());
    }
    double Get(double time) override
    {
        return aws[dist(gen)](gen) + time;
    }
};

inline double GetExponentialDelay(double intensity, double time)
{
    auto aws = std::exponential_distribution<double>(intensity);
    return aws(gen) + time;
}

inline double NextDouble()
{
    return distribution(gen);
}

//};
#endif