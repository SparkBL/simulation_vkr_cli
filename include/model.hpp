#ifndef MODEL_HPP
#define MODEL_HPP
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "router.hpp"
#include "producer.hpp"

double Time;
double End;
double Interval;

struct Model
{
	std::unordered_map<std::string, Producer *> Components;
	std::vector<double> event_queue;
	double time;
	double end;
	std::unordered_map<std::string, Router *> Routers;
	void Init()
	{
		event_queue.reserve(10);
	}

	void AddConnection(Producer *from, std::string from_slot, Producer *to, std::string to_slot)
	{
		Router *r;
		if (Routers.count(to->Tag() + to_slot))
			r = Routers.at(to->Tag() + to_slot);
		else
		{
			r = new Router();
			Routers.insert(std::pair<std::string, Router *>(to->Tag() + to_slot, r));
		}
		to->InputAtConnect(to_slot, r);
		from->OutputAtConnect(from_slot, r);
	}

	void AddProducer(Producer *producer, std::string label)
	{
		Components.insert(std::pair<std::string, Producer *>(label, producer));
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