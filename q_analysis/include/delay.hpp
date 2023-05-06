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
        // Description gen
        std::ostringstream ss;
        ss << "Exponential delay generator" << std::endl
           << "Parameters:" << std::endl
           << "# Intensity = " << intensity;
        description = ss.str();
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

        // Description gen
        std::ostringstream ss;
        ss << "Weibull delay generator" << std::endl
           << "Parameters:" << std::endl
           << "# A = " << a << std::endl
           << "# B = " << b;
        description = ss.str();
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

        // Description gen
        std::ostringstream ss;
        ss << "Lognormal delay generator" << std::endl
           << "Parameters:" << std::endl
           << "# M = " << m << std::endl
           << "# S = " << s;
        description = ss.str();
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

        // Description gen
        std::ostringstream ss;
        ss << "Uniform delay generator" << std::endl
           << "Parameters:" << std::endl
           << "# A = " << a << std::endl
           << "# B = " << b;
        description = ss.str();
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

        // Description gen
        std::ostringstream ss;
        ss << "Gamma delay generator" << std::endl
           << "Parameters:" << std::endl
           << "# K = " << k << std::endl
           << "# Theta = " << teta_;
        description = ss.str();
    }
    double Get(double time) override
    {
        return aws(gen) + time;
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