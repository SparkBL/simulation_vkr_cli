#ifndef DELAY_HPP
#define DELAY_HPP
#include "env.hpp"
#include "math.h"
#include <ctime>
#include <random>
#include <vector>
#include <time.h>

const double float64EqualityThreshold = 1e-11;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<double> distribution(0, 1);
class Delay
{
public:
    virtual double Get() = 0;
};

class ExpDelay : public Delay
{
    double Intensity;

public:
    ExpDelay(double intensity)
    {
        this->Intensity = intensity;
    }
    double Get()
    {
        auto aws = std::exponential_distribution<double>(Intensity);
        return aws(gen) + Time;
    }
};

class UniformDelay : public Delay
{
    double A, B;

public:
    UniformDelay(double A, double B)
    {
        this->A = A;
        this->B = B;
    }
    double Get()
    {
        auto aws = std::uniform_real_distribution<double>(A, B);
        return aws(gen) + Time;
    }
};

double ExponentialDelay(double intensity)
{
    auto aws = std::exponential_distribution<double>(intensity);
    return aws(gen) + Time;
}

bool almostEqual(double a, double b)
{
    return abs(a - b) <= float64EqualityThreshold;
}

double NextDouble()
{
    return distribution(gen);
}

#endif