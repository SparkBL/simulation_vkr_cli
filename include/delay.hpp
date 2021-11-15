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
    std::exponential_distribution<double> aws;

public:
    ExpDelay(double intensity)
    {
        this->Intensity = intensity;
        aws = std::exponential_distribution<double>(Intensity);
    }
    double Get()
    {
        return aws(gen) + Time;
    }
};

class UniformDelay : public Delay
{
    double A, B;
    std::uniform_real_distribution<double> aws;

public:
    UniformDelay(double A, double B)
    {
        this->A = A;
        this->B = B;
        aws = std::uniform_real_distribution<double>(A, B);
    }
    double Get()
    {
        return aws(gen) + Time;
    }
};

class GammaDelay : public Delay
{
    double K, Teta;
    std::gamma_distribution<double> aws;

public:
    GammaDelay(double K, double Teta)
    {
        this->K = K;
        this->Teta = Teta;
        aws = std::gamma_distribution<double>(K, Teta);
    }
    double Get()
    {
        return aws(gen) + Time;
    }
};

double ExponentialDelay(double intensity)
{
    auto aws = std::exponential_distribution<double>(intensity);
    return aws(gen) + Time;
}

double NextDouble()
{
    return distribution(gen);
}

#endif