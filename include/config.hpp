#ifndef CONFIG_H
#define CONFIG_H
#include "json.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

struct Config
{
    std::string InputType;
    std::string SigmaDelayType;
    double Sigma;
    double SigmaA, SigmaB;
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

std::ostream &operator<<(std::ostream &os, const Config &conf)
{
    std::stringstream LL, QQ;
    for (const auto &i : conf.L)
        LL << i << ";";
    for (const auto &i : conf.Q)
    {
        for (const auto &j : i)
            QQ << j << ";";
        QQ << std::endl;
    }
    return os << "Input type: " << conf.InputType << std::endl
              << "Sigma delay type: " << conf.SigmaDelayType << std::endl
              << "Sigma: " << conf.Sigma << std::endl
              << "Sigma A: " << conf.SigmaA << std::endl
              << "Sigma B: " << conf.SigmaB << std::endl
              << "Lambda simple: " << conf.LSimple << std::endl
              << "Mu 1: " << conf.Mu1 << std::endl
              << "Mu 2: " << conf.Mu2 << std::endl
              << "Alpha: " << conf.Alpha << std::endl
              << "End: " << conf.End << std::endl
              << "Interval: " << conf.Interval << std::endl
              << "Output file: " << conf.OutFilename << std::endl
              << "Lambda: " << std::endl
              << LL.str() << std::endl
              << "Q: " << std::endl
              << QQ.str() << std::endl;
}

Config ParseConfig(std::string fileName)
{
    std::ifstream in(fileName);
    using json = nlohmann::json;
    json j_complete;
    in >> j_complete;
    Config conf;
    conf.InputType = j_complete.value("input_type", "mmpp");
    conf.SigmaDelayType = j_complete.value("sigma_delay_type", "exp");
    conf.Sigma = j_complete.value("sigma", 0.4);
    conf.SigmaA = j_complete.value("sigmaA", 0.3);
    conf.SigmaB = j_complete.value("sigmaB", 0.4);
    conf.L = j_complete.value("L", std::vector<double>{0, 0, 0});
    conf.LSimple = j_complete.value("LSimple", 1);
    conf.Q = j_complete.value("Q", std::vector<std::vector<double>>{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}});
    conf.Mu1 = j_complete.value("mu1", 5);
    conf.Mu2 = j_complete.value("mu2", 2.5);
    conf.Alpha = j_complete.value("alpha", 0.8);
    conf.End = j_complete.value("end", 100000);
    conf.Interval = j_complete.value("interval", 5);
    conf.OutFilename = j_complete.value("output_file", "out.csv");
    return conf;
}
#endif // CONFIG_H
