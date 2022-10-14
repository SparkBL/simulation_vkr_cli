#ifndef UTILS_HPP
#define UTILS_HPP
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <cassert>
void exportMatrix(std::vector<std::vector<double>> unr, std::string filename)
{
    std::ofstream file(filename);

    for (size_t i = 0; i < unr.size(); i++)
    {
        for (size_t j = 0; j < unr[i].size(); j++)
        {
            if (!(unr[i][j] > 0.0000000))
                unr[i][j] = 0;
            if (j + 1 == unr[i].size())
                file << std::setprecision(7) << unr[i][j];
            else
                file << std::setprecision(7) << unr[i][j] << ";";
        }
        file << std::endl;
    }
    file.close();
}

void exportVector(std::vector<double> unr, std::string filename)
{
    std::ofstream file(filename);
    for (size_t i = 0; i < unr.size(); i++)
    {
        if (!(unr[i] > 0.0000000))
            unr[i] = 0;
        file << std::setprecision(7) << unr[i] << '\n';
    }
    file.close();
}

void exportChars(std::vector<std::string> labels, std::vector<double> values, std::string filename)
{
    std::ofstream file(filename);
    file << "characteristic"
         << ";"
         << "value" << '\n';
    for (size_t i = 0; i < labels.size(); i++)
    {
        file << labels[i] << ";" << values[i] << '\n';
    }
    file.close();
}

template <typename TK, typename TV>
std::vector<TK> extract_keys(std::unordered_map<TK, TV> const &input_map)
{
    std::vector<TK> retval;
    for (auto const &element : input_map)
    {
        retval.push_back(element.first);
    }
    return retval;
}

template <typename... Args>
std::string string_sprintf(const char *format, Args... args)
{
    int length = std::snprintf(nullptr, 0, format, args...);
    assert(length >= 0);

    char *buf = new char[length + 1];
    std::snprintf(buf, length + 1, format, args...);

    std::string str(buf);
    delete[] buf;
    return str;
}
#endif
