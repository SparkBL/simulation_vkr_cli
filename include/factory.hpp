#ifndef FACTORY_HPP
#define FACTORY_HPP
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>

#include "json.hpp"
#include "delay.hpp"
#include "node.hpp"
#include "orbit.hpp"
#include "stream.hpp"
#include "stats.hpp"
// std::map<std::string, typename> creation_mapping();

class DelayFactory
{
public:
    virtual ~DelayFactory() {} // Allow proper inheritance
    virtual Delay *Create(nlohmann::json parameters) = 0;
};

class ExponentialDelayFactory : public DelayFactory
{
    ExponentialDelay *Create(nlohmann::json parameters) override
    {
        std::string a = parameters.dump();
        double intensity = parameters.value("intensity", 0.0);
        return new ExponentialDelay(intensity);
    }
};

class UniformDelayFactory : public DelayFactory
{
    UniformDelay *Create(nlohmann::json parameters) override
    {
        double a = parameters.value("a", 0.0);
        double b = parameters.value("b", 0.0);
        return new UniformDelay(a, b);
    }
};

class GammaDelayFactory : public DelayFactory
{
    GammaDelay *Create(nlohmann::json parameters) override
    {
        double k = parameters.value("k", 0.0);
        double teta = parameters.value("teta", 0.0);
        return new GammaDelay(k, teta);
    }
};

// must be loaded
class DelayTypeFactory
{
private:
    inline static std::unordered_map<std::string, DelayFactory *> factories_;

public:
    static void RegisterFactory(std::string delay_type, DelayFactory *delay_factory)
    {
        factories_.insert(std::pair<std::string, DelayFactory *>(delay_type, delay_factory));
    }

    static Delay *Create(std::string delayType, nlohmann::json parameters)
    {
        return factories_.at(delayType)->Create(parameters);
    }

    static std::vector<std::string> GetTypes()
    {
        std::vector<std::string> result;
        for (auto &t : factories_)
        {
            result.push_back(t.first);
        }
        return result;
    }
};

class ProducerFactory
{
public:
    virtual ~ProducerFactory() {} // Allow proper inheritance
    virtual Producer *Create(nlohmann::json parameters) = 0;
};

class RQTNodeFactory : public ProducerFactory
{
    RQTNode *Create(nlohmann::json parameters) override
    {
        Delay *d1 = DelayTypeFactory::Create(parameters.at("mu_1").value("delay_type", "exponential"), parameters.at("mu_1"));
        Delay *d2 = DelayTypeFactory::Create(parameters.at("mu_2").value("delay_type", "exponential"), parameters.at("mu_2"));
        return new RQTNode(d1, d2);
    }
};

class RQNodeFactory : public ProducerFactory
{
    RQNode *Create(nlohmann::json parameters) override
    {
        Delay *d = DelayTypeFactory::Create(parameters.at("mu").value("delay_type", "exponential"), parameters.at("mu"));
        return new RQNode(d);
    }
};

class SimpleNodeFactory : public ProducerFactory
{
    SimpleNode *Create(nlohmann::json parameters) override
    {
        Delay *d = DelayTypeFactory::Create(parameters.at("mu").value("delay_type", "exponential"), parameters.at("mu"));
        return new SimpleNode(d);
    }
};

class MMPPFactory : public ProducerFactory
{
    MMPP *Create(nlohmann::json parameters) override
    {
        std::vector<double> l = parameters.value("L", std::vector<double>{0, 0, 0});
        std::vector<std::vector<double>> q = parameters.value("Q", std::vector<std::vector<double>>{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}});
        int request_type = parameters.value("request_type", 1);
        return new MMPP(l, q, request_type);
    }
};

class SimpleInputFactory : public ProducerFactory
{
    SimpleInput *Create(nlohmann::json parameters) override
    {
        Delay *d1 = DelayTypeFactory::Create(parameters.at("l").value("delay_type", "exponential"), parameters.at("l"));
        int request_type = parameters.value("request_type", 1);
        return new SimpleInput(d1, request_type);
    }
};

class OrbitFactory : public ProducerFactory
{
    IOrbit *Create(nlohmann::json parameters) override
    {
        Delay *d = DelayTypeFactory::Create(parameters.at("sigma").value("delay_type", "exponential"), parameters.at("sigma"));
        return new Orbit(d);
    }
};

class StatCollectorFactory : public ProducerFactory
{
    StatCollector *Create(nlohmann::json parameters) override
    {
        double interval = parameters.value("interval", 10);
        return new StatCollector(interval);
    }
};

// must be loaded
class ProducerTypeFactory
{
private:
    inline static std::unordered_map<std::string, ProducerFactory *> factories_;

public:
    static void RegisterFactory(std::string producer_type, ProducerFactory *producer_factory)
    {
        factories_.insert(std::pair<std::string, ProducerFactory *>(producer_type, producer_factory));
    }

    static Producer *Create(std::string producerType, nlohmann::json parameters)
    {
        return factories_.at(producerType)->Create(parameters);
    }

    static std::vector<std::string> GetTypes()
    {
        std::vector<std::string> result;
        for (auto &t : factories_)
        {
            result.push_back(t.first);
        }
        return result;
    }
};

#endif