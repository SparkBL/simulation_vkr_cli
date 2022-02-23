#ifndef ENV_HPP
#define ENV_HPP
#include <vector>
#include <unordered_map>
#include <algorithm>
//namespace env
//{

class Model
{
private:
	std::unordered_map<std::string, Producer *> producers_;
	std::unordered_map<std::string, Router *> routers_;
	std::unordered_map<std::string, Producer *> collectors_;

public:
	void AddElement(std::string label, Producer *producer)
	{
		producers_.insert(std::pair<std::string, Producer *>(label, producer));
	}

	void AddConnection(std::string from_label, std::string from_slot, std::string to_label, std::string to_slot)
	{
		Router *r;
		if (routers_.count(to_label + to_slot))
			r = routers_.at(to_label + to_slot);
		else
		{
			Router *r = new Router();
			routers_.insert(std::pair<std::string, Router *>(to_label + to_slot, r));
		}
		producers_.at(to_label)->SlotAt(to_slot)->Connect(r);
		producers_.at(from_label)->SlotAt(from_slot)->Connect(r);
	}
};

std::vector<double> EventQueue;
double Time;
double End;

Model Init(double end)
{
	EventQueue.reserve(10);
	Time = 0;
	End = end;
	return Model();
};

#endif
