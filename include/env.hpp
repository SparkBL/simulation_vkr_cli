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
std::vector<double> EventQueue;
void Init()
{
	EventQueue.reserve(10);
}
double Time;
double End;
double Interval;

class Worker
{
	double Time;
	double End;
	double Interval;
	std::vector<double> EventQueue;
};

#endif