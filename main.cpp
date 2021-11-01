#include <iostream>
#include <fstream>
#include <chrono>
#include "delayprovider.h"
#include "system.h"
#include <memory>
#include <thread>

void export3DPlot(std::vector<std::vector<double>> unr, int rowSize)
{
    std::ofstream file;
    file.open("example.csv");

    for (int i = 0; i < rowSize; i++)
    {
        for (int j = 0; j < rowSize; j++)
        {
            if (!(unr[i][j] > 0.0000000))
                unr[i][j] = 0;
            if (j + 1 == rowSize)
                file << unr[i][j];
            else
                file << unr[i][j] << ";";
        }
        file << '\n';
    }
    file.close();
}

/*void export2DPlot()
{
    std::vector<double> unr = stats->GetSummaryDistribution();
    out.setCodec("UTF-8");
    for (size_t i = 0; i < unr.size(); i++)
    {
        if (!(unr[i] > 0.0000000))
            unr[i] = 0;
        out << unr[i] << '\n';
    }
    file.close();
}*/

int main(int argc, char *argv[])
{
    std::vector<std::vector<double>> shiftProbs = {{-0.2, 0.1, 0.1}, {0.3, -0.5, 0.2}, {0.2, 0.4, -0.6}};
    std::vector<double> lambdas = {0.94, 1.25, 1.56};
    double mu1 = 2.0;
    double mu2 = 1.5;
    double alpha = 0.8;
    double time = 6000000;
    double interval = 5.0;
    double lambda = 1.0;
    int matrixSize = 3;
	Environment::Init();
    std::unique_ptr<RQSystem> sys;
    DelayProvider *sigmaDelay = sigmaDelay = new ExponentialDelay();

    double sigmaDelayIntensity = 0.4;
    double sigmaDelayIntensity1 = 0.1;
    const Statistic *stats = nullptr;

    bool useMMPP = true;
    Environment::SetStatInterval(interval);
    Environment::SetMaxTime(time);

    DelayProvider *d = new ExponentialDelay();
    Processable *input;
    Processable *calledInput = new SimpleInputProcess(d, mu2, EventType::Called);
    Orbit *orbit = new Orbit(sigmaDelay, sigmaDelayIntensity, sigmaDelayIntensity1);
    INode *node = new CalledClaimsNode(d, mu1, mu2);
    /*if (!useMMPP)
    {
        input = new SimpleInputProcess(d, lambda, EventType::FromInputProcess);
        sys.reset(new RQSystem(input, calledInput, orbit, node));
        stats = sys->GetStatistics();
    }
    else
    {*/
    input = new MMPPInputProcess(d, shiftProbs, lambdas, matrixSize, EventType::FromInputProcess);
    sys.reset(new RQSystem(input, calledInput, orbit, node));
    stats = sys->GetStatistics();
    // }
    //Env actions
    Environment::SetAction([&sys]
                           { return sys->Process() != nullptr; });
    Environment::SetIntervalAction([stats]
                                   { stats->Stat(); });
    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
    auto t1 = high_resolution_clock::now();
    std::thread thd([]
                    {
                        while (!Environment::isFinished())
                        {
                            Environment::NextMoment();
                        }
                    });
    thd.join();
    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> elapsed = t2 - t1;
    std::cout << "Elapsed - " << elapsed.count() / 60.0 << "s";
    export3DPlot(stats->GetDistribution(), stats->GetDistributionRowSize());
    return 0;
}
