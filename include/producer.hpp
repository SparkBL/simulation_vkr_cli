#ifndef PRODUCER_HPP
#define PRODUCER_HPP
#include "router.hpp"
#include "utils.hpp"
#include "string"
#include "vector"
class Producer
{

public:
    std::vector<double> queue;
    virtual ~Producer() {}
    virtual std::vector<double> Produce(double time) = 0;
    virtual std::string Tag() { return "base"; }
    virtual void InputAtConnect(std::string slot_name, Router *router)
    {
        return this->inputs_.find(slot_name)->second->Connect(router);
    }

    virtual void OutputAtConnect(std::string slot_name, Router *router)
    {

        return this->outputs_.find(slot_name)->second->Connect(router);
    }

    virtual std::vector<std::string> Inputs()
    {
        return extract_keys<std::string, InSlot *>(inputs_);
    }
    virtual std::vector<std::string> Outputs()
    {
        return extract_keys<std::string, OutSlot *>(outputs_);
    }

protected:
    std::unordered_map<std::string, InSlot *> inputs_ = {};
    std::unordered_map<std::string, OutSlot *> outputs_ = {};

    std::vector<double> GetEvents()
    {
        std::vector<double> ret = queue;
        queue = {};
        return ret;
    }
};

#endif