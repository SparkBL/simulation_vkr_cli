#ifndef CONFIG_H
#define CONFIG_H
#include "json.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
struct Config
{
    std::string InputType;
    std::string SigmaDelayType;
    double Sigma;
    double SigmaA, SigmaB;
    double SigmaGammaK, SigmaGammaTeta;
    std::vector<double> L;
    double LSimple;
    std::vector<std::vector<double>> Q;
    double Mu1;
    double Mu2;
    double Alpha;
    double End;
    double Interval;
    std::string OutFilename;
};

Config ParseConfig(std::string fileName)
{

    return Config{};
}
#endif // CONFIG_H
