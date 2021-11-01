#include "utils.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<double> distribution(0, 1);

double utils::NextDouble()
{
    return distribution(gen);
}

double utils::StandardDistribution()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> distribution(0, 1);
    return distribution(gen);
}
