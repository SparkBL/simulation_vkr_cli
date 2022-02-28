#include <iostream>
#include <fstream>
#include <chrono>
#include <memory>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>
#include "components.hpp"

int main(int argc, char *argv[])
{
    std::vector<std::string> args(argv, argv + argc);

    std::ifstream in(args[1], std::ios::in);
    using json = nlohmann::json;
    json params;
    in >> params;
    json elements = params.at("elements");
    json flow = params.at("flow");
    double end = params.value("end", 0);

    Model model = Init(end);

    //Registering factories
    DelayTypeFactory::RegisterFactory("exponential", new ExponentialDelayFactory());
    DelayTypeFactory::RegisterFactory("uniform", new UniformDelayFactory());
    DelayTypeFactory::RegisterFactory("gamma", new GammaDelayFactory());

    ProducerTypeFactory::RegisterFactory("simple_node", new SimpleNodeFactory());
    ProducerTypeFactory::RegisterFactory("rq_node", new RQNodeFactory());
    ProducerTypeFactory::RegisterFactory("rqt_node", new RQTNodeFactory());
    ProducerTypeFactory::RegisterFactory("simple_input", new RQTNodeFactory());
    ProducerTypeFactory::RegisterFactory("mmpp_input", new RQTNodeFactory());
    ProducerTypeFactory::RegisterFactory("orbit", new OrbitFactory());

    ProducerTypeFactory::RegisterFactory("output_collector", new TOutputStatCollectorFactory());

    for (auto &e : elements)
    {
        std::string label = e.at("label").get<std::string>();
        std::string type = e.at("type").get<std::string>();
        if (label.size() < 1)
        {
            std::cout << "Label is too short: " << label << std::endl;
            return;
        }
        model.AddElement(label, ProducerTypeFactory::Create(type, e.at("parameters")));
    }

    for (auto &e : flow)
    {
        std::string label_from = e.at("from").get<std::string>();
        std::string label_to = e.at("to").get<std::string>();
        std::string slot_from = e.at("slot_from").get<std::string>();
        std::string slot_to = e.at("slot_to").get<std::string>();
        model.AddConnection(label_from, slot_from, label_to, slot_to);
    }

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

    auto t2 = high_resolution_clock::now();
    exitSignal.set_value();
    logging.join();
    //  stat.join();
    duration<double, std::milli> elapsed = t2 - t1;
    // std::cout << "Elapsed - " << elapsed.count() / 1000 << "s" << std::endl
    //           << "Mean input - " << statCollector.GetMeanInput() << "; Mean called - " << statCollector.GetMeanCalled();
    // export3DPlot(statCollector.GetDistribution(), args[2]);
    // export2DPlot(statCollector.GetSummaryDistribution(), "summary" + args[2]);
    return 0;
}
