#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include "utils.h"
#include "event.h"
#include <string>
#include <functional>
#include <set>

class Environment
{
public:
    static int NextMoment();
    static void Init();
    static void SetAction(std::function<int(void)> callback);
    static void SetIntervalAction(std::function<void(void)> callback);
    static double Time();
    static double StatInterval();
    static double MaxTime();
    static void SetStatInterval(double interval);
    static void SetMaxTime(double max);
    static bool isFinished();

private:
    static std::function<int(void)> mainAction;
    static std::function<void(void)> intervalAction;
    static double time;
    static double nextStatTime;
    static double statInterval;
    static double maxTime;
    static std::vector<Event *> completedEvents;
};

#endif // ENVIRONMENT_H
