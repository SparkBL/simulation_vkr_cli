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

std::ostream &operator<<(std::ostream &os, const Config &conf)
{
    std::stringstream LL, QQ;
    for (const auto &i : conf.L)
        LL << std::right << std::setw(10) << i;
    for (const auto &i : conf.Q)
    {
        for (const auto &j : i)
            QQ << std::right << std::setw(10) << j;
        QQ << std::endl;
    }
    return os << "Input type: " << std::setw(40 - std::strlen("Input type: ")) << std::right << conf.InputType << std::endl
              << "Sigma delay type: " << std::setw(40 - std::strlen("Sigma delay type: ")) << std::right << conf.SigmaDelayType << std::endl
              << "Sigma: " << std::setw(40 - std::strlen("Sigma: ")) << std::right << conf.Sigma << std::endl
              << "Sigma A: " << std::setw(40 - std::strlen("Sigma A: ")) << std::right << conf.SigmaA << std::endl
              << "Sigma B: " << std::setw(40 - std::strlen("Sigma B: ")) << std::right << conf.SigmaB << std::endl
              << "Sigma Gamma K: " << std::setw(40 - std::strlen("Sigma Gamma K: ")) << std::right << conf.SigmaGammaK << std::endl
              << "Sigma Gamma Teta: " << std::setw(40 - std::strlen("Sigma Gamma Teta: ")) << std::right << conf.SigmaGammaTeta << std::endl
              << "Lambda simple: " << std::setw(40 - std::strlen("Lambda simple: ")) << std::right << conf.LSimple << std::endl
              << "Mu 1: " << std::setw(40 - std::strlen("Mu 1: ")) << std::right << conf.Mu1 << std::endl
              << "Mu 2: " << std::setw(40 - std::strlen("Mu 2: ")) << std::right << conf.Mu2 << std::endl
              << "Alpha: " << std::setw(40 - std::strlen("Alpha: ")) << std::right << conf.Alpha << std::endl
              << "End: " << std::setw(40 - std::strlen("End: ")) << std::right << conf.End << std::endl
              << "Interval: " << std::setw(40 - std::strlen("Interval: ")) << std::right << conf.Interval << std::endl
              << "Output file: " << std::setw(40 - std::strlen("Output file: ")) << std::right << conf.OutFilename << std::endl
              << "Lambda: " << std::setw(40 - std::strlen("Lambda: ")) << std::right << std::endl
              << std::setw(40 - LL.str().length()) << std::right << LL.str() << std::endl
              << "Q: " << std::setw(40 - std::strlen("Q: ")) << std::right << std::endl
              << std::setw(40 - QQ.str().length()) << std::right << QQ.str() << std::endl;
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
    conf.SigmaA = j_complete.value("sigma_a", 0.3);
    conf.SigmaB = j_complete.value("sigma_b", 0.4);
    conf.SigmaGammaK = j_complete.value("sigma_gamma_k", 0.0);
    conf.SigmaGammaTeta = j_complete.value("sigma_gamma_teta", 0.0);
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
