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
};

Config ParseConfig(std::string fileName)
{
    std::ifstream in(fileName);
    std::string contents((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());
    using json = nlohmann::json;
    json j_complete = json::parse(contents);
    Config conf;
    conf.InputType = j_complete.value("input_type", "mmpp");
    conf.SigmaDelayType = j_complete.value("sigma_delay_type", "exp");
}
#endif // CONFIG_H
