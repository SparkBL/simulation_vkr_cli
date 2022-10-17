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
	Model(double end = 1000)
	{
		time = 0;
		this->end = end;
		components = {};
		routers = {};
		event_queue = {};
	}

	void AddConnection(std::string from_producer, std::string from_slot, std::string to_producer, std::string to_slot)
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
	}

	void AddProducer(Producer *producer, std::string label)
	{
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