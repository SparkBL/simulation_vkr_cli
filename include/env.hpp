#ifndef ENV_HPP
#define ENV_HPP
#include <vector>
#include <algorithm>
//namespace env
//{
std::vector<double> EventQueue;
void Init()
{
	EventQueue.reserve(10);
}
double Time;
double End;

/*struct ModelEnv
	{
		static double Time;
		static double End;
		std::vector<double> EventQueue;
	};*/
//};
#endif
