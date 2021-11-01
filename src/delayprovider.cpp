#include "delayprovider.h"

double ExponentialDelay::GetDelay(double intensity, double intensity1) const
{
    auto aws = std::exponential_distribution<double>(intensity);
    return aws(gen) + Environment::Time();
}

double UniformDelay::GetDelay(double intensity, double intensity1) const
{
    auto aws = std::uniform_real_distribution<double>(intensity, intensity1);
    return aws(gen) + Environment::Time();
}
