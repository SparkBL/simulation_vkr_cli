#ifndef PRODUCER_HPP
#define PRODUCER_HPP
#include "string"
#include "vector"
#include "router.hpp"
#include "utils.hpp"
class Producer
{

public:
    std::vector<double> queue;
    virtual ~Producer() = default;
    virtual std::vector<double> Produce(double time) = 0;
    virtual std::string Tag() { return "base"; }
    virtual void InputAtConnect(std::string slot_name, Router *router)
    {
        if (router == nullptr)
        {
            throw std::invalid_argument("router is nil");
        }

        if (!this->inputs.count(slot_name))
        {
            throw std::invalid_argument(slot_name + " not found in input slot list");
        }
        return this->inputs.find(slot_name)->second->Connect(router);
    }

    virtual void OutputAtConnect(std::string slot_name, Router *router)
    {
        if (router == nullptr)
        {
            throw std::invalid_argument("router is nil");
        }

        if (!this->outputs.count(slot_name))
        {
            throw std::invalid_argument(slot_name + " not found in output slot list");
        }
        return this->outputs.find(slot_name)->second->Connect(router);
    }

    virtual std::vector<std::string> Inputs()
    {
        return extract_keys<std::string, InSlot *>(inputs);
    }
    virtual std::vector<std::string> Outputs()
    {
        return extract_keys<std::string, OutSlot *>(outputs);
    }

protected:
    std::unordered_map<std::string, InSlot *> inputs = {};
    std::unordered_map<std::string, OutSlot *> outputs = {};

    std::vector<double> GetEvents()
    {
        std::vector<double> ret = queue;
        queue = {};
        return ret;
    }
};

#endif