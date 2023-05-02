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
	std::unordered_map<std::string, Producer &> components;
	std::unordered_map<std::string, Router *> routers;

	Model(double end = 1000)
	{
		time = 0;
		this->end = end;
		components = {};
		routers = {};
		event_queue = {};
	}

	std::vector<double> Queue()
	{
		return event_queue;
	}
	void SetTime(double t)
	{
		if (t < 0)
			throw std::invalid_argument("model time must be greater than zero");
		time = t;
	}
	void SetEnd(double t)
	{
		if (t < 0)
			throw std::invalid_argument("model time must be greater than zero");
		end = t;
	}

	double Time()
	{
		return time;
	}

	double End()
	{
		return end;
	}

	void Flush()
	{
		event_queue.clear();
		for (auto &e : routers)
		{
			e.second->Flush();
		}
	}

	const std::unordered_map<std::string, Producer &> &Components() const
	{
		const std::unordered_map<std::string, Producer &> &p = components;
		return p;
	}

	const std::unordered_map<std::string, Router *> &Routers() const
	{
		const std::unordered_map<std::string, Router *> &p = routers;
		return p;
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

	Router &RouterAt(std::string label)
	{
		if (!routers.count(label))
		{
			throw std::invalid_argument(label + " not found in routers");
		}
		return *routers.at(label);
	}

	Producer &ComponentAt(std::string label)
	{
		if (!components.count(label))
		{
			throw std::invalid_argument(label + " not found in components");
		}
		return components.at(label);
	}

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
		Router *r;
		if (routers.count(q))
			r = routers.at(q);
		else
		{
			r = new Router();
			routers.insert(std::pair<std::string, Router *>(q, r));
		}
		components.at(to_producer).InputAtConnect(to_slot, *r);
		components.at(from_producer).OutputAtConnect(from_slot, *r);
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
		Router *r;
		if (routers.count(q))
			r = routers.at(q);
		else
		{
			r = new Router();
			routers.insert(std::pair<std::string, Router *>(q, r));
		}
		components.at(to_producer).InputAtConnect(to_slot, *r);
		return q;
	}

	std::string AddHangingOutput(std::string from_producer, std::string from_slot)
	{
		if (!components.count(from_producer))
		{
			throw std::invalid_argument(from_producer + " not found in components");
		}
		std::ostringstream ss;
		ss << "ho:" << from_producer << ":" << from_slot << ":";
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
		components.at(from_producer).OutputAtConnect(from_slot, *r);
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
		components.at(from_producer).OutputAtConnect(from_slot, *r);
		return q;
	}

	/*	void AddConnectionReader(std::string connection, std::string label, RouterReader &r)
		{
			if (!routers.count(connection))
			{
				throw std::invalid_argument(connection + " not found in routers");
			}
			routers.at(connection)->AddReader(r, label);
		}
	*/

	void AddProducer(Producer &producer, std::string label)
	{
		if (components.count(label))
		{
			throw std::invalid_argument(label + " already exists in components");
		}
		// components.insert(std::pair<std::string, Producer *>(label, producer));
		components.insert(components.end(), std::pair<std::string, Producer &>(label, producer));
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