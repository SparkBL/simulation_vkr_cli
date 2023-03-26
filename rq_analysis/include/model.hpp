#ifndef MODEL_HPP
#define MODEL_HPP
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "router.hpp"
#include "producer.hpp"
#include "utils.hpp"
#include <sstream>

class Model
{
public:
	std::vector<double> event_queue;
	double time;
	double end;
	std::unordered_map<std::string, Producer *> components;
	std::unordered_map<std::string, Router *> routers;
	Model(double end = 1000)
	{
		time = 0;
		this->end = end;
		components = {};
		routers = {};
		event_queue = {};
	}

	/*void AddConnection(std::string from_producer, std::string from_slot, std::string to_producer, std::string to_slot)
	{
		Router *r;
		if (routers.count(to_producer + "_" + to_slot))
			r = routers.at(to_producer + "_" + to_slot);
		else
		{
			r = new Router();
			routers.insert(std::pair<std::string, Router *>(to_producer + "_" + to_slot, r));
		}
		components.at(to_producer)->InputAtConnect(to_slot, r);
		components.at(from_producer)->OutputAtConnect(from_slot, r);
	}*/

	std::string AddConnection(std::string from_producer, std::string from_slot, std::string to_producer, std::string to_slot)
	{
		if (!components.count(from_producer))
		{
			throw std::invalid_argument(from_producer + " not found in components");
		}
		if (!components.count(to_producer))
		{
			throw std::invalid_argument(to_producer + " not found in components");
		}

		std::ostringstream ss;
		ss << from_producer << ":" << from_slot << ":" << to_producer << ":" << to_slot;
		std::string q = ss.str();
		//	std::string q = string_sprintf("%s:%s:%s:%s", to_producer, to_slot, from_producer, from_slot);
		Router *r;
		if (routers.count(q))
			r = routers.at(q);
		else
		{
			r = new Router();
			routers.insert(std::pair<std::string, Router *>(q, r));
		}
		components.at(to_producer)->InputAtConnect(to_slot, r);
		components.at(from_producer)->OutputAtConnect(from_slot, r);
		return q;
	}

	std::string AddHangingInput(std::string to_producer, std::string to_slot)
	{
		if (!components.count(to_producer))
		{
			throw std::invalid_argument(to_producer + " not found in components");
		}
		std::ostringstream ss;
		ss << "i:" << to_producer << ":" << to_slot;
		std::string q = ss.str();
		//	std::string q = string_sprintf("%s:%s:%s:%s", to_producer, to_slot, from_producer, from_slot);
		Router *r;
		if (routers.count(q))
			r = routers.at(q);
		else
		{
			r = new Router();
			routers.insert(std::pair<std::string, Router *>(q, r));
		}
		components.at(to_producer)->InputAtConnect(to_slot, r);
		return q;
	}

	std::string AddHangingOutput(std::string from_producer, std::string from_slot)
	{
		if (!components.count(from_producer))
		{
			throw std::invalid_argument(from_producer + " not found in components");
		}
		std::ostringstream ss;
		ss << "o:" << from_producer << ":" << from_slot;
		std::string q = ss.str();
		//	std::string q = string_sprintf("%s:%s:%s:%s", to_producer, to_slot, from_producer, from_slot);
		Router *r;
		if (routers.count(q))
			r = routers.at(q);
		else
		{
			r = new Router();
			routers.insert(std::pair<std::string, Router *>(q, r));
		}
		components.at(from_producer)->OutputAtConnect(from_slot, r);
		return q;
	}

	std::string AddHangingOutputNoQueue(std::string from_producer, std::string from_slot)
	{
		if (!components.count(from_producer))
		{
			throw std::invalid_argument(from_producer + " not found in components");
		}
		std::ostringstream ss;
		ss << "onq:" << from_producer << ":" << from_slot;
		std::string q = ss.str();
		Router *r;
		if (routers.count(q))
			r = routers.at(q);
		else
		{
			r = new OutputRouter();
			routers.insert(std::pair<std::string, Router *>(q, r));
		}
		components.at(from_producer)->OutputAtConnect(from_slot, r);
		return q;
	}

	void AddProducer(Producer *producer, std::string label)
	{
		if (components.count(label))
		{
			throw std::invalid_argument(label + " already exists in components");
		}
		if (producer == nullptr)
		{
			throw std::invalid_argument("producer object is nil");
		}

		// components.insert(std::pair<std::string, Producer *>(label, producer));
		components.insert(components.end(), std::pair<std::string, Producer *>(label, producer));
		// components[label] = producer;
	}

	double NextStep()
	{
		if (!event_queue.empty())
		{

			auto min = std::min_element(std::begin(event_queue), std::end(event_queue),
										[](double c1, double c2)
										{
											return c1 < c2;
										});
			time = *min;
			event_queue.erase(min);
		}
		return time;
	}

	bool IsDone()
	{
		return time >= end;
	}

	void Aggregate(std::vector<double> events)
	{
		event_queue.insert(event_queue.end(), events.begin(), events.end());
	}
};

#endif