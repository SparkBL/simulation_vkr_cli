#include <iostream>
#include <fstream>
#include <chrono>
#include <memory>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>
#include "include/components.hpp"

void export3DPlot(std::vector<std::vector<double>> unr)
{
    std::ofstream file("example.csv");

    for (int i = 0; i < unr.size(); i++)
    {
        for (int j = 0; j < unr[i].size(); j++)
        {
            if (!(unr[i][j] > 0.0000000))
                unr[i][j] = 0;
            if (j + 1 == unr[i].size())
                file << unr[i][j];
            else
                file << unr[i][j] << ";";
        }
        file << std::endl;
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
    std::vector<std::vector<double>> Q = {{-0.2, 0.1, 0.1}, {0.3, -0.5, 0.2}, {0.2, 0.4, -0.6}};
    std::vector<std::vector<double>> L = {{0.94, 0, 0}, {0, 1.25, 0}, {0, 0, 1.56}};
    double mu1 = 2.0;
    double mu2 = 1.5;
    double alpha = 0.8;
    double end = 6000000;
    double interval = 5.0;
    double lambda = 1.0;
    double sigmaDelayIntensity = 0.4;
    double sigmaDelayA = 0.1;
    double sigmaDelayB = 0.4;
    Router *inputChannel = new Router();
    Router *orbitChannel = new Router();
    Router *orbitAppendChannel = new Router();
    Router *outputChannel = new Router();
    Router *calledChannel = new Router();

    // SimpleInput inStream(new ExpDelay(lambda), TypeInput, inputChannel);
    MMPP inStream(L, Q, TypeInput, inputChannel);
    ExpDelay *sigmaDelay = new ExpDelay(sigmaDelayIntensity);

    SimpleInput callStream(new ExpDelay(alpha), TypeCalled, calledChannel);
    Orbit orbit(sigmaDelay, orbitChannel, orbitAppendChannel);
    Node node(new ExpDelay(mu1), new ExpDelay(mu2), inputChannel, calledChannel, orbitChannel, orbitAppendChannel, outputChannel);
    Time = 0;
    End = end;
    Interval = interval;
    StatCollector statCollector(outputChannel);
    std::cout << "Parameters set. Starting...\n";

    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
    auto t1 = high_resolution_clock::now();
    std::promise<void> exitSignal;
    std::future<void> futureObj = exitSignal.get_future();
    std::thread logging([&futureObj]
                        {
                            while (futureObj.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout)
                            {
                                using namespace std::chrono_literals;
                                std::cout << "\r\e[K" << std::flush << "Time passed - " << Time;
                                std::this_thread::sleep_for(1000ms);
                            }
                            std::cout << std::endl;
                        });

    /* std::thread stat([&futureObj, &statCollector]
                     {
                         while (futureObj.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout)
                         {
                             statCollector.GatherStat();
                         }
                     });*/

    while (Time < End)
    {
        statCollector.GatherStat();
        inStream.Produce();
        orbit.Produce();
        callStream.Produce();
        node.Produce();
        orbit.Append();
        if (EventQueue.size() > 0)
        {
            std::sort(EventQueue.begin(), EventQueue.end());
            Time = EventQueue[0];
            EventQueue.erase(EventQueue.begin());
        }
    }
    auto t2 = high_resolution_clock::now();
    exitSignal.set_value();
    logging.join();
    //stat.join();
    duration<double, std::milli> elapsed = t2 - t1;
    std::cout << "Elapsed - " << elapsed.count() / 1000 << "s";
    export3DPlot(statCollector.GetDistribution());
    return 0;
}
