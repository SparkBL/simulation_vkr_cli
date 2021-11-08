#include <iostream>
#include <fstream>
#include <chrono>
#include <memory>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>
#include "include/components.hpp"

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
    std::vector<std::string> args(argv, argv + argc);

    Config conf = ParseConfig(args[1]);

    Router *inputChannel = new Router();
    Router *orbitChannel = new Router();
    Router *orbitAppendChannel = new Router();
    Router *outputChannel = new Router();
    Router *calledChannel = new Router();
    Stream *inStream;
    if (conf.InputType == "mmpp")
        inStream = new MMPP(conf.L, conf.Q, TypeInput, inputChannel);
    if (conf.InputType == "simple")
        inStream = new SimpleInput(new ExpDelay(conf.LSimple), TypeInput, inputChannel);

    ExpDelay *sigmaDelay = new ExpDelay(conf.Sigma);
    SimpleInput callStream(new ExpDelay(conf.Alpha), TypeCalled, calledChannel);
    Orbit orbit(sigmaDelay, orbitChannel, orbitAppendChannel);
    Node node(new ExpDelay(conf.Mu1), new ExpDelay(conf.Mu2), inputChannel, calledChannel, orbitChannel, orbitAppendChannel, outputChannel);
    Time = 0;
    End = conf.End;
    Interval = conf.Interval;
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
    Init();
    while (Time < End)
    {
        statCollector.GatherStat();
        inStream->Produce();
        orbit.Produce();
        callStream.Produce();
        node.Produce();
        orbit.Append();
        if (!EventQueue.empty())
        {
            auto min = std::min_element(std::begin(EventQueue), std::end(EventQueue),
                                        [](double c1, double c2)
                                        {
                                            return c1 < c2;
                                        });
            Time = *min;
            EventQueue.erase(min);
        }
    }
    auto t2 = high_resolution_clock::now();
    exitSignal.set_value();
    logging.join();
    //  stat.join();
    duration<double, std::milli> elapsed = t2 - t1;
    std::cout << "Elapsed - " << elapsed.count() / 1000 << "s" << std::endl
              << "Mean input - " << statCollector.GetMeanInput() << "; Mean called - " << statCollector.GetMeanCalled();
    export3DPlot(statCollector.GetDistribution(), args[2]);
    return 0;
}
