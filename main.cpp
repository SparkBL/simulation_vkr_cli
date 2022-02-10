#include <iostream>
#include <fstream>
#include <chrono>
#include <memory>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>
#include "components.hpp"

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

int main(int argc, char *argv[])
{
    std::vector<std::string> args(argv, argv + argc);

    std::ifstream in(args[1], std::ios::in);
    using json = nlohmann::json;
    json j_complete;
    in >> j_complete;
    json elements = j_complete.at("elements");
    std::map<std::string, Producer *> producers;
    std::map<std::string, Orbit *> orbits;
    std::map<std::string, Router *> routers;
    std::vector<StatCollector *> stats;
    for (auto &element : elements)
    {
        if (element.at("label").get<std::string>().size() < 1)
        {
            std::cout << "Label is too short: " << element.at("label").get<std::string>() << std::endl;
            return;
        }
        SWITCH(element.value("type", ""))
        {
            CASE("simple_i") :
            {
                double intensity = element.at("parameters").value("intensity", 0.0);
                Router *r = new NoneRouter();
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_output", r));
                Producer *s = new SimpleInput(new ExponentialDelay(intensity), typeInput, new NoneRouter());
                producers.insert(std::pair<std::string, Producer *>(element.value("label", ""), s));
                break;
            }
            CASE("mmpp_i") :
            {
                Router *r = new NoneRouter();
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_output", r));
                Producer *s = new MMPP(element.at("parameters").value("L", std::vector<double>{0, 0, 0}), element.at("parameters").value("Q", std::vector<std::vector<double>>{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}), typeInput, r);
                producers.insert(std::pair<std::string, Producer *>(element.value("label", ""), s));
                break;
            }
            CASE("simple_n") :
            {
                Router *r = new NoneRouter(), *r1 = new NoneRouter();
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_input", r));
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_output", r1));
                double intensity = element.at("parameters").value("intensity", 0.0);
                Producer *n = new SimpleNode(new ExponentialDelay(intensity), r, r1);
                producers.insert(std::pair<std::string, Producer *>(element.value("label", ""), n));
                break;
            }
            CASE("rq_n") :
            {
                Router *r = new NoneRouter(), *r1 = new NoneRouter(), *r2 = new NoneRouter(), *r3 = new NoneRouter();
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_input", r));
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_output", r1));
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_orbit_input", r2));
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_orbit_output", r3));
                double intensity = element.at("parameters").value("intensity", 0.0);
                Producer *n = new RQNode(new ExponentialDelay(intensity), r, r2, r3, r1);
                producers.insert(std::pair<std::string, Producer *>(element.value("label", ""), n));
                break;
            }
            CASE("rqt_n") :
            {
                Router *r = new NoneRouter(), *r1 = new NoneRouter(), *r2 = new NoneRouter(), *r3 = new NoneRouter(), *r4 = new NoneRouter();
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_input", r));
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_output", r1));
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_orbit_input", r2));
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_orbit_output", r3));
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_called_input", r4));
                double intensity = element.at("parameters").value("intensity", 0.0);
                double called_intensity = element.at("parameters").value("called_intensity", 0.0);
                Producer *n = new RQTNode(new ExponentialDelay(intensity), new ExponentialDelay(called_intensity), r, r4, r2, r3, r1);
                producers.insert(std::pair<std::string, Producer *>(element.value("label", ""), n));
                break;
            }
            CASE("base_o") :
            {
                Router *r = new NoneRouter(), *r1 = new NoneRouter();
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_input", r));
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_output", r1));
                double intensity = element.at("parameters").value("intensity", 0.0);
                Producer *n = new Orbit(new ExponentialDelay(intensity), r1, r);
                producers.insert(std::pair<std::string, Producer *>(element.value("label", ""), n));
                break;
            }
            CASE("state_o") :
            {
                Router *r = new NoneRouter(), *r1 = new NoneRouter(), *s = new NoneRouter();
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_input", r));
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_output", r1));
                routers.insert(std::pair<std::string, Router *>(element.value("label", "") + "_state", s));
                double intensity = element.at("parameters").value("intensity", 0.0);
                Producer *n = new StateOrbit(new ExponentialDelay(intensity), r1, r, s);
                producers.insert(std::pair<std::string, Producer *>(element.value("label", ""), n));
                break;
            }
        default:
        {
            std::cout << "Unrecognized elements type: " << element.value("type", "") << std::endl;
            return;
        }
        }
    }
    json flow = j_complete.at("flow");
    for (auto &path : flow)
    {
        Router *r = new Router();
        delete routers.at(path.value("from", "") + "_" + path.value("from_slot", ""));
        routers.at(path.value("from", "") + "_" + path.value("from_slot", "")) = r;
        delete routers.at(path.value("to", "") + "_" + path.value("to_slot", ""));
        routers.at(path.value("to", "") + "_" + path.value("to_slot", "")) = r;
    }
    json stat = j_complete.at("stats");
    for (auto &e : stat)
    {
        SWITCH(e.value("type", ""))
        {
            CASE("td_output") :
            {
                StatCollector *r = new TOutputStatCollector(routers[e.value("element", "") + e.value("slot", "")], e.value("interval", 5));
                stats.push_back(r);
                break;
            }
            CASE("orbit") :
            {
                StatCollector *r = new OrbitStatCollector(routers[e.value("element", "") + e.value("slot", "")]);
                stats.push_back(r);
                break;
            }
        }
    }

    Time = 0;
    End = j_complete.value("end", 0);

    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
    auto t1 = high_resolution_clock::now();
    std::promise<void> exitSignal;
    std::future<void> futureObj = exitSignal.get_future();
    std::thread logging([&futureObj]
                        {
                            std::cout << "Parameters set. Starting...\n";
                            while (futureObj.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout)
                            {

                                using namespace std::chrono_literals;
                                std::cout << "\r\e[K" << std::flush << "Time passed - " << Time;
                                std::this_thread::sleep_for(1000ms);
                            }
                            std::cout << std::endl;
                        });
    Init();
    while (Time < End)
    {
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
        inStream->Produce();
        orbit.Produce();
        callStream.Produce();
        node.Produce();
        orbit.Append();
        statCollector.GatherStat();
    }
    auto t2 = high_resolution_clock::now();
    exitSignal.set_value();
    logging.join();
    //  stat.join();
    duration<double, std::milli> elapsed = t2 - t1;
    std::cout << "Elapsed - " << elapsed.count() / 1000 << "s" << std::endl
              << "Mean input - " << statCollector.GetMeanInput() << "; Mean called - " << statCollector.GetMeanCalled();
    export3DPlot(statCollector.GetDistribution(), args[2]);
    export2DPlot(statCollector.GetSummaryDistribution(), "summary" + args[2]);
    return 0;
}
