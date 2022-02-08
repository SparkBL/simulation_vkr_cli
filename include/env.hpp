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

#endif