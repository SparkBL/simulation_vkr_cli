#ifndef MODEL_HPP
#define MODEL_HPP
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "router.hpp"
#include "producer.hpp"

class Model
{
public:
	std::vector<double> event_queue;
	double time;
	double end;
	std::unordered_map<std::string, Producer *> components;
	std::unordered_map<std::string, Router *> routers;
	Model()
	{
		components = {};
		routers = {};
		event_queue.reserve(10);
	}

	void AddConnection(Producer *from, std::string from_slot, Producer *to, std::string to_slot)
	{
		Router *r;
		if (routers.count(to->Tag() + "_" + to_slot))
			r = routers.at(to->Tag() + "_" + to_slot);
		else
		{
			r = new Router();
			routers.insert(std::pair<std::string, Router *>(to->Tag() + "_" + to_slot, r));
		}
		to->InputAtConnect(to_slot, r);
		from->OutputAtConnect(from_slot, r);
	}

	void AddProducer(Producer *producer, std::string label)
	{
		// components.insert(std::pair<std::string, Producer *>(label, producer));
		components.insert(components.end(), std::pair<std::string, Producer *>(label, producer));
		// components[label] = producer;
	}

	double NextStep()
	{
		double t = 0;
		if (!event_queue.empty())
		{
			auto min = std::min_element(std::begin(event_queue), std::end(event_queue),
										[](double c1, double c2)
										{
											return c1 < c2;
										});
			t = *min;
			event_queue.erase(min);
		}
		return t;
	}

	void Aggregate(std::vector<double> events)
	{
		event_queue.insert(event_queue.end(), events.begin(), events.end());
	}
};

#endif