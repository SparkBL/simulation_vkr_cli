#include <iostream>
#include <fstream>
#include <chrono>
#include <memory>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>

#include "env.hpp"
#include "orbit.hpp"
#include "stats.hpp"
#include "utils.hpp"
#include "factory.hpp"

int main(int argc, char *argv[])
{
    // stringify args
    std::vector<std::string> args(argv, argv + argc);

    std::ifstream in(args[1], std::ios::in);
    using json = nlohmann::json;
    json params;
    in >> params;

    json input_param = params.at("input");
    json called_input_param = params.at("called_input");
    json node_param = params.at("node");
    json orbit_param = params.at("orbit");
    json stat_param = params.at("stat");
    std::string prefix = params.at("export_prefix").get<std::string>();
    std::string export_dir = params.at("export_dir").get<std::string>();
    std::vector<std::string> exports = params.at("exports").get<std::vector<std::string>>();

    DelayTypeFactory::RegisterFactory("exponential", new ExponentialDelayFactory());
    DelayTypeFactory::RegisterFactory("uniform", new UniformDelayFactory());
    DelayTypeFactory::RegisterFactory("gamma", new GammaDelayFactory());

    ProducerTypeFactory::RegisterFactory("simple_node", new SimpleNodeFactory());
    ProducerTypeFactory::RegisterFactory("rq_node", new RQNodeFactory());
    ProducerTypeFactory::RegisterFactory("rqt_node", new RQTNodeFactory());
    ProducerTypeFactory::RegisterFactory("simple_input", new SimpleInputFactory());
    ProducerTypeFactory::RegisterFactory("mmpp_input", new MMPPFactory());
    ProducerTypeFactory::RegisterFactory("orbit", new OrbitFactory());

    ProducerTypeFactory::RegisterFactory("stat_collector", new StatCollectorFactory());

    Producer
        *input = ProducerTypeFactory::Create(input_param.at("type").get<std::string>(), input_param.at("parameters")),
        *called_input = ProducerTypeFactory::Create(called_input_param.at("type").get<std::string>(), called_input_param.at("parameters")),
        *node = ProducerTypeFactory::Create(node_param.at("type").get<std::string>(), node_param.at("parameters")),
        *stat_collector = ProducerTypeFactory::Create(stat_param.at("type").get<std::string>(), stat_param.at("parameters"));
    IOrbit *orbit = static_cast<IOrbit *>(ProducerTypeFactory::Create(orbit_param.at("type").get<std::string>(), orbit_param.at("parameters")));

    AddConnection(input, "out_slot", node, "in_slot");
    AddConnection(called_input, "out_slot", node, "called_slot");
    AddConnection(orbit, "orbit_slot", node, "orbit_slot");
    AddConnection(node, "orbit_append_slot", orbit, "orbit_append_slot");
    AddConnection(node, "out_slot", stat_collector, "in_slot");
    Time = 0;
    End = params.at("end").get<double>();

    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
    auto t1 = high_resolution_clock::now();
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
        input->Produce();
        orbit->Produce();
        called_input->Produce();
        node->Produce();
        orbit->Append();
        stat_collector->Produce();
    }
    auto t2 = high_resolution_clock::now();

    duration<double, std::milli> elapsed = t2 - t1;
    StatCollector *stat_collector_c = static_cast<StatCollector *>(stat_collector);
    if (std::find(exports.begin(), exports.end(), "distr") != exports.end())
        exportMatrix(stat_collector_c->GetDistribution(), "./" + export_dir + "/" + prefix + "distr.csv");

    if (std::find(exports.begin(), exports.end(), "summary_distr") != exports.end())
        exportVector(stat_collector_c->GetSummaryDistribution(), "./" + export_dir + "/" + prefix + "summary_distr.csv");

    if (std::find(exports.begin(), exports.end(), "chars") != exports.end())
    {
        std::vector<std::string> labels = {"elapsed", "mean_input", "mean_called", "variation_input", "variation_called"};
        std::vector<double> values = {elapsed.count() / 1000, stat_collector_c->GetMeanInput(), stat_collector_c->GetMeanCalled(), stat_collector_c->GetVariationIntervalInput(), stat_collector_c->GetVariationIntervalCalled()};
        exportChars(labels, values, "./" + export_dir + "/" + prefix + "chars.csv");
    }

    return 0;
}
