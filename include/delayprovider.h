#ifndef DELAYPROVIDER_H
#define DELAYPROVIDER_H
#include "utils.h"
#include "environment.h"

class DelayProvider
{
public:
    virtual double GetDelay(double intensity, double intensity1 = 0) const = 0;
    virtual ~DelayProvider(){};
};

class ExponentialDelay : public DelayProvider
{
    double GetDelay(double intensity, double intensity1 = 0) const;
};

class UniformDelay : public DelayProvider
{
    double GetDelay(double intensity, double intensity1 = 0) const;
};

#endif // DELAYPROVIDER_H
