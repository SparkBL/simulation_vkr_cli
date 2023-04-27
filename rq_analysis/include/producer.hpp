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
    virtual void InputAtConnect(std::string slot_name, Router &router)
    {
        if (!this->inputs.count(slot_name))
        {
            throw std::invalid_argument(slot_name + " not found in input slot list");
        }
        this->inputs.erase(slot_name);
        this->inputs.insert(std::pair<std::string, InSlot>(slot_name, InSlot(router)));
    }

    virtual void OutputAtConnect(std::string slot_name, Router &router)
    {

        if (!this->outputs.count(slot_name))
        {
            throw std::invalid_argument(slot_name + " not found in output slot list");
        }
        this->inputs.erase(slot_name);
        this->outputs.insert(std::pair<std::string, OutSlot>(slot_name, OutSlot(router)));
    }

    virtual std::vector<std::string> Inputs()
    {
        return extract_keys<std::string, InSlot>(inputs);
    }
    virtual std::vector<std::string> Outputs()
    {
        return extract_keys<std::string, OutSlot>(outputs);
    }

protected:
    std::unordered_map<std::string, InSlot> inputs = {};
    std::unordered_map<std::string, OutSlot> outputs = {};

    std::vector<double> GetEvents()
    {
        std::vector<double> ret = queue;
        queue = {};
        return ret;
    }
};

#endif