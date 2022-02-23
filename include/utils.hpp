#ifndef STREAM_HPP
#define STREAM_HPP
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

void export3DPlot(std::vector<std::vector<double>> unr, std::string filename)
{
    std::ofstream file(filename);

    for (int i = 0; i < unr.size(); i++)
    {
        for (int j = 0; j < unr[i].size(); j++)
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

void export2DPlot(std::vector<double> unr, std::string filename)
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

#endif