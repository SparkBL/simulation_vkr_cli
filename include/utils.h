#ifndef UTILS_H
#define UTILS_H
#include <ctime>
#include <random>
#include <vector>
#include <time.h>
namespace utils
{
    double NextDouble();
    double StandardDistribution();
}

extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_real_distribution<double> distribution;

#endif // UTILS_H
