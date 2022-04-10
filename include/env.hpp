/*
type Process interface {
	Produce()
}*/
/*
func Init() {
	Time = 0
}
*/
#ifndef ENV_HPP
#define ENV_HPP
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "router.hpp"
#include "producer.hpp"

std::vector<double>
	EventQueue;
void Init()
{
	EventQueue.reserve(10);
}
double Time;
double End;
double Interval;
std::unordered_map<std::string, Router *> Routers;

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
	to->SlotAt(to_slot)->Connect(r);
	from->SlotAt(from_slot)->Connect(r);
}

#endif