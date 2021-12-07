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
#include <algorithm>

class Producer
{
public:
	virtual ~Producer(){};
	virtual void Produce() = 0;
};

std::vector<double>
	EventQueue;
void Init()
{
	EventQueue.reserve(10);
}
double Time;
double End;
double Interval;

#endif