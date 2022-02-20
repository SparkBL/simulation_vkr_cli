#ifndef MAPPING_HPP
#define MAPPING_HPP
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>

#include "delay.hpp"
#include "node.hpp"
#include "orbit.hpp"
#include "stream.hpp"
//std::map<std::string, typename> creation_mapping();

class ProducerFactory
{
public:
    virtual ~ProducerFactory() {} // Allow proper inheritance
    virtual Producer *Create() = 0;
};

class RQTNodeFactory : public ProducerFactory
{
    template <class... ArgTypes>
    RQTNode *Create(ArgTypes... args) override
    {
        return new RQTNode(args);
    }
};

class ProducerTypeFactory
{
private:
    static std::unordered_map<std::string, ProducerFactory *> factories_;

public:
    static void RegisterFactory(std::string producer_type, ProducerFactory *producer_factory)
    {
        factories_.insert(std::pair<std::string, ProducerFactory *>(producer_type, producer_factory));
    }

    static Producer *Create(std::string vehicleType)
    {
        return factories_.at(vehicleType)->Create();
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
